#include "BtcTransactionTracer.h"

#include <cassert>
#include <future>

#include <plog/Log.h>
#include <nlohmann/json.hpp>

namespace btc_explorer {
	using namespace nlohmann;

	namespace {
		string extract_outs(string&& text) {
			size_t start = text.find("\"out\":[");
			if (start == string::npos) return "{}"; // Shouldn't happen

			size_t cnt = 1, i = start + 7;
			for (; cnt && i < text.size(); ++i) {
				if (text[i] == '[') ++cnt;
				else if (text[i] == ']') --cnt;
			}

			if (cnt) return "{}"; // Shouldn't happen
			return "{" + text.substr(start, i - start) + "}";
		}

		bool outs_filter(int depth, json::parse_event_t event, json& parsed) {
			if (depth == 3 && event == json::parse_event_t::key && parsed != "addr" &&
				parsed != "spending_outpoints" && parsed != "spent" && parsed != "tx_index")
				return false;
			else if (depth == 5 && event == json::parse_event_t::key && parsed != "tx_index")
				return false;
			return true;
		}
	}

	void BtcTransactionTracer::processTxResponse(cpr::Response&& r, IdType txid) {
		if (r.error || r.status_code != 200) {
			PLOGW << "Unable to read txid=" << txid << " from " << r.url;

			if (r.error) {
				PLOGW << "Details: err_code=" << static_cast<int>(r.error.code) << ",msg=" << r.error.message;
			}
			else {
				PLOGW << "Details: http_code=" << r.status_code << ",msg=" << r.status_line;
			}

			std::lock_guard<std::mutex> lk(mx);
			tx_err.insert(std::move(txid));
			return;
		}

		auto j = json::object();
		try {
			j = json::parse(extract_outs(std::move(r.text)), outs_filter);
			PLOGD << "Processing txid=" << txid << " with " << j["out"].size() << " outs";
		}
		catch (std::exception& ex) {
			PLOGW << "Processing txid failed with " << ex.what();
			std::lock_guard<std::mutex> lk(mx);
			tx_err.insert(txid);
		}

		bool all_spent = true;
		for (auto& tx : j["out"]) {
			assert(tx["spent"].is_boolean());

			if (tx["spent"] == true) {
				for (auto& addr : tx["spending_outpoints"]) {
					assert(addr["tx_index"].is_number_unsigned());

					auto val = addr["tx_index"].get<IdType>();
					std::lock_guard<std::mutex> lk(mx);
					q.push(val);

					PLOGD << "Chaining new transaction with txid=" << val;
				}
			}
			else {
				auto val = tx["addr"];
				if (val.is_string()) {
					// cb4ba354741eb3ff2a44dfe410136c7a268af85e5f8ec261185aef0f0167270a, "addr":null ?!
					all_spent = false;

					auto addr = val.get<string>();
					std::lock_guard<std::mutex> lk(mx);
					res.insert(addr);

					PLOGD << "Adding new unspent address=" << addr;
				}
			}
		}
		PLOGD << "\n";

		if (!all_spent) {
			std::lock_guard<std::mutex> lk(mx);
			tx_cache.insert(j["out"].front()["tx_index"].get<IdType>());
		}
	}

	std::optional<BtcTransactionTracer::IdType> BtcTransactionTracer::getNextAddress() {
		std::lock_guard<std::mutex> lk(mx);
		while (!q.empty() && tx_cache.count(q.front())) q.pop();
		if (q.empty()) return std::nullopt;

		auto txid = std::move(q.front()); q.pop();
		return txid;
	}

	bool BtcTransactionTracer::init(string&& tx_hash) {
		PLOGI << "Starting tracing from the tx_hash=" << tx_hash << "\n";
		auto r = btcApi->getTxRaw(tx_hash);

		if (r.error)
			PLOGF << "Unable to start the tracing: " << "err=" << static_cast<int>(r.error.code) << ",msg=" << r.error.message;
		else if (r.status_code != 200)
			PLOGF << "Unable to start the tracing: " << "err=" << r.status_code << ",msg=" << r.status_line;
		else {
			auto j = json::parse(r.text, [](int depth, json::parse_event_t event, json& parsed) {
				if (depth > 1 || depth == 1 && event == json::parse_event_t::key && parsed != json("tx_index"))
					return false;
				return true;
				});

			auto val = j["tx_index"];
			if (val.is_number_unsigned()) q.push(val.get<std::uint64_t>());
		}
		return r.error.code == cpr::ErrorCode::OK && r.status_code == 200;
	}

	pair<std::vector<BtcTransactionTracer::AddressType>, std::vector<BtcTransactionTracer::IdType>>
		BtcTransactionTracer::traceAddresses(string tx_hash) {
		if (!init(std::move(tx_hash))) return {};

		std::vector<std::future<void>> requests(async_cnt);
		for (uint64_t step = 1, total_req = 0; !q.empty(); ++step) {
			PLOGI << "#Step " << step << ": total_req=" << total_req << ", res_size=" << res.size();

			auto qsize = min(q.size(), async_cnt);
			for (auto i = 0; i < qsize; ++i) {
				auto txid = getNextAddress();
				if (!txid) qsize = i;
				else {
					requests[i] = btcApi->getTxRawAsync(
						*txid, std::bind(&BtcTransactionTracer::processTxResponse, this, std::placeholders::_1, *txid));
				}
			}

			for (auto i = 0; i < qsize; ++i) {
				try {
					requests[i].get();
				}
				catch (std::exception& ex) {
					PLOGE << "Error encountered in the async request:" << ex.what();
				}
			}
			total_req += qsize;
		}

		return {
			std::vector<AddressType>(res.begin(), res.end()),
			std::vector<IdType>(tx_err.begin(), tx_err.end())
		};
	}

	BtcTransactionTracer::BtcTransactionTracer(const TracerConfig& conf) :
		btcApi(std::make_unique<BtcApi>()), async_cnt(conf.async_cnt) {

		PLOGI << "Initialized max async requests=" << static_cast<size_t>(async_cnt);
	}

}