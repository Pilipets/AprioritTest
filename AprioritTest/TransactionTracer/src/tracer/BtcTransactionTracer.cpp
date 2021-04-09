#include "BtcTransactionTracer.h"

#include "client/BtcApiClient.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <cassert>
#include <future>

using std::cout;
using namespace nlohmann;

void BtcTransactionTracer::processTxResponse(cpr::Response&& r, string txid) {
	if (r.error.code != cpr::ErrorCode::OK) {
		std::lock_guard<std::mutex> lk(mx);
		cout << "Error: " << r.error.message << "\n";
		err_res.insert(std::move(txid));
	}
	else {
		auto body = json::parse(r.text);

		for (auto& tx : body["out"]) {
			cout << tx << "\n\n";
			if (tx["spent"] == true) {
				assert(tx["spending_outpoints"].size() == 1);

				for (auto& addr : tx["spending_outpoints"]) {
					auto val = addr["tx_index"];
					assert(val.is_number_unsigned());
					std::lock_guard<std::mutex> lk(mx);
					cout << std::to_string(val.get<std::uint64_t>()) << "\n";
					q.push(std::to_string(val.get<std::uint64_t>()));
				}
			} else {
				auto val = tx["addr"];
				assert(val.is_string());

				std::lock_guard<std::mutex> lk(mx);
				res.insert(val.get<std::string>());
			}
		}
	}
}

string BtcTransactionTracer::getNextAddress() {
	std::lock_guard<std::mutex> lk(mx);
	auto txid = std::move(q.front()); q.pop();
	return txid;
}

std::pair<std::vector<string>, std::vector<string>> BtcTransactionTracer::traceAddresses(string txid) {
	q.push(std::move(txid));

	std::vector<std::future<void>> requests(async_cnt);
	for (int step = 1; step < 5 && !q.empty(); ++step) {
		auto qsize = min(q.size(), async_cnt);
		for (auto i = 0; i < qsize; ++i) {
			auto txid = getNextAddress();
			requests[i] = btcApi->getTxRawAsync(
				txid,
				std::bind(&BtcTransactionTracer::processTxResponse, this, std::placeholders::_1, txid)
			);
		}

		for (auto i = 0; i < qsize; ++i)
			requests[i].get();
	}

	return { std::vector<string>(res.begin(), res.end()),
		std::vector<string>(err_res.begin(), err_res.end()) };
}

BtcTransactionTracer::BtcTransactionTracer(std::shared_ptr<BtcApi> btcApi, const TracerConfig& conf):
	btcApi(btcApi), async_cnt(conf.async_cnt){

	assert(conf.async_cnt);
}
