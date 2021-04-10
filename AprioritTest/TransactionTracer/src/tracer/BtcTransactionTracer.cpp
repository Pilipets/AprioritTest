#include "BtcTransactionTracer.h"

#include <cassert>
#include <future>
#include <fstream>

#include <plog/Log.h>
#include <nlohmann/json.hpp>

namespace btc_explorer {
	using namespace nlohmann;
	using boost::asio::post;

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

	void BtcTransactionTracer::processTxResponse(cpr::Response&& r, IdType txid, size_t depth) {
		auto j = json::object();
		try {
			j = json::parse(extract_outs(std::move(r.text)), outs_filter);
			PLOGD << "Processing txid=" << txid << " with " << j["out"].size() << " outs";
		}
		catch (std::exception& ex) {
			std::lock_guard<std::mutex> lk(mx);
			tx_err.insert(txid);
			PLOGW_IF(tx_err.size() % 100 == 0) << "Processing txid failed with " << ex.what();
			return;
		}

		for (auto& tx : j["out"]) {
			assert(tx["spent"].is_boolean());

			if (tx["spent"] == true) {
				for (auto& addr : tx["spending_outpoints"]) {
					assert(addr["tx_index"].is_number_unsigned());

					auto val = addr["tx_index"].get<IdType>();
					std::lock_guard<std::mutex> lk(mx);
					if (depth < conf.max_depth && !tx_cache.count(val)) {
						PLOGD << "Chaining new transaction with txid=" << val << ", depth= " << depth;
						tx_cache.insert(val);
						post(pool, std::bind(&BtcTransactionTracer::processTxRequest, this, val, depth + 1));
					}
				}
			}
			else {
				auto val = tx["addr"];
				if (val.is_string()) {
					// cb4ba354741eb3ff2a44dfe410136c7a268af85e5f8ec261185aef0f0167270a, "addr":null ?!
					auto addr = val.get<string>();

					std::lock_guard<std::mutex> lk(mx);
					if (!res.count(addr)) {
						res.insert(addr);
						PLOGI_IF(res.size() % 100 == 0) << "Adding new unspent address=" << addr << ", " << res.size() << " in total" << ", cache_size= " << tx_cache.size();
					}
				}
			}
		}
		PLOGD << "\n";
	}

	void BtcTransactionTracer::processTxRequest(IdType txid, size_t depth) {
		auto r = btcApi->getTxRaw(txid);
		if (r.error || r.status_code != 200) {
			{
				std::lock_guard<std::mutex> lk(mx);
				tx_err.insert(std::move(txid));
				PLOGW_IF(tx_err.size() % 100 == 0) << "Unable to read txid=" << txid << " from " << r.url;
			}

			if (r.error) {
				PLOGW << "Details: err_code=" << static_cast<int>(r.error.code) << ",msg=" << r.error.message;
			}
			else {
				PLOGW << "Details: http_code=" << r.status_code << ",msg=" << r.status_line;
			}
		}
		else {
			processTxResponse(std::move(r), txid, depth);
		}
	}

	std::optional<BtcTransactionTracer::IdType> BtcTransactionTracer::init(string&& tx_hash) {
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
			if (val.is_number_unsigned())
				return val.get<std::uint64_t>();
		}
		return std::nullopt;
	}

	pair<std::vector<BtcTransactionTracer::AddressType>, std::vector<BtcTransactionTracer::IdType>>
		BtcTransactionTracer::traceAddresses(string tx_hash) {
		std::ofstream fout(conf.out_file);

		auto txid = init(std::move(tx_hash));
		if (!txid) return {};

		tx_cache.insert(*txid);
		post(pool, std::bind(&BtcTransactionTracer::processTxRequest, this, *txid, 0));
		pool.join();

		std::copy(res.begin(), res.end(), std::ostream_iterator<AddressType>(fout, ", "));
		return {
			std::vector<AddressType>(res.begin(), res.end()),
			std::vector<IdType>(tx_err.begin(), tx_err.end())
		};
	}

	BtcTransactionTracer::BtcTransactionTracer(const TracerConfig& conf) :
		btcApi(std::make_unique<BtcApi>()), pool(min(std::thread::hardware_concurrency(), conf.async_cnt)),
		conf(conf) {

		PLOGI << "Initialized thread pool with workers=" << min(std::thread::hardware_concurrency(), conf.async_cnt);
	}

	BtcTransactionTracer::~BtcTransactionTracer() {
		pool.stop();
		pool.join();
	}
}