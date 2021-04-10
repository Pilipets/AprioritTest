#pragma once
#include <memory>
#include <queue>
#include <string>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <utility>
#include <optional>
#include <limits>

#include <cpr/response.h>
#include <boost/asio.hpp>
#include <boost/unordered_set.hpp>

#include "client/BtcApiClient.h"

namespace btc_explorer {
	using std::pair;
	using std::string;

	struct TracerConfig {
		uint8_t async_cnt = std::thread::hardware_concurrency();
		const char* out_file = nullptr;
		size_t max_depth = 1000;
	};


	class BtcTransactionTracer {
		typedef BtcApiClient BtcApi;
		typedef uint64_t IdType;
		typedef string AddressType;

		const std::unique_ptr<BtcApi> btcApi;

		boost::asio::thread_pool pool;
		std::mutex mx;
		boost::unordered_set<IdType> tx_cache;

		boost::unordered_set<IdType> tx_err;
		boost::unordered_set<AddressType> res;

		TracerConfig conf;
	private:

		void processTxResponse(cpr::Response&& r, IdType txid, size_t depth);
		void processTxRequest(IdType txid, size_t depth);

		std::optional<IdType> init(string&& tx_hash);

	public:
		BtcTransactionTracer() = delete;
		pair<std::vector<AddressType>, std::vector<IdType>> traceAddresses(string txHash);
		BtcTransactionTracer(const TracerConfig& conf);
		~BtcTransactionTracer();
	};
}