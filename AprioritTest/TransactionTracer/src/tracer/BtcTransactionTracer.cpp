#include "BtcTransactionTracer.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <cassert>
#include <future>

using std::cout;
using namespace nlohmann;

void BtcTransactionTracer::processTxResponse(cpr::Response&& r, IdType txid) {
	/*
	if (r.error.code != cpr::ErrorCode::OK) {
		std::lock_guard<std::mutex> lk(mx);
		//cout << "Error: " << r.error.message << "\n";
		err_cache.insert(std::move(txid));
	}
	else {
		auto body = json::parse(r.text);

		for (auto& tx : body["out"]) {
			//cout << tx << "\n\n";
			if (tx["spent"] == true) {
				assert(tx["spending_outpoints"].size() == 1);

				for (auto& addr : tx["spending_outpoints"]) {
					auto val = addr["tx_index"];
					assert(val.is_number_unsigned());
					std::lock_guard<std::mutex> lk(mx);
					//cout << std::to_string(val.get<std::uint64_t>()) << "\n";
					auto id = val.get<std::uint64_t>();
					if (!cache.count(id)) {
						q.push(std::to_string(id));
						cache.insert(id);
					}
				}
			} else {
				auto val = tx["addr"];
				if (val.is_string()) {
					// cb4ba354741eb3ff2a44dfe410136c7a268af85e5f8ec261185aef0f0167270a, "addr":null ?!
					//cout << body << "\n";
					//cout << val.type_name() << "\n";


					std::lock_guard<std::mutex> lk(mx);
					res.insert(val.get<std::string>());
				}
				//assert(val.is_string());
			}
		}
	}*/
}

BtcTransactionTracer::IdType BtcTransactionTracer::getNextAddress() {
	std::lock_guard<std::mutex> lk(mx);
	auto txid = std::move(q.front()); q.pop();
	return txid;
}

bool BtcTransactionTracer::init(string&& tx_hash) {
	cout << "Initializing tracins fund from " << tx_hash << "\n";
	auto r = btcApi->getTxRaw(tx_hash);

	if (r.status_code != 200) {
		cout << "Unable to initialize the process for " << tx_hash << "\n";
		json j = {
			{"http", {"code", r.status_code}, {"msg", r.status_line}},
			{"err",  {"code", r.error.code}, {"msg", r.error.message}}
		};
		cout << j << "\n";
	}
	return false;
}

std::vector<string> BtcTransactionTracer::traceAddresses(string tx_hash) {
	if (!init(std::move(tx_hash))) return {};

	std::vector<std::future<void>> requests(async_cnt);
	for (uint64_t step = 1, total_req = 0; !q.empty(); ++step) {
		cout << "#Step " << step << ":{";
		cout << "total requests=" << total_req << ",cache_size= " << cache.size() << ",";
		cout << "res_size=" << res.size() << ",err_size= " << err_cache.size() << ";\n";

		/*
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
		total_req += qsize;*/
	}

	return std::vector<string>(res.begin(), res.end());
}

BtcTransactionTracer::BtcTransactionTracer(const TracerConfig& conf):
	btcApi(std::make_unique<BtcApi>()), async_cnt(conf.async_cnt){

	assert(conf.async_cnt);
}
