#pragma once
#include <memory>
#include <queue>
#include <string>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <utility>
#include <optional>

#include <cpr/response.h>
#include <boost/asio.hpp>

#include "client/BtcApiClient.h"

namespace btc_explorer {
	using std::pair;
	using std::string;

	struct TracerConfig {
		uint8_t async_cnt;
		const char* out_file;
	};


	class BtcTransactionTracer {
		typedef BtcApiClient BtcApi;
		typedef uint64_t IdType;
		typedef string AddressType;

		const std::unique_ptr<BtcApi> btcApi;

		boost::asio::thread_pool pool;
		std::mutex mx;
		std::unordered_set<IdType> tx_cache;

		std::unordered_set<IdType> tx_err;
		std::unordered_set<AddressType> res;

		TracerConfig conf;
	private:

		void processTxResponse(cpr::Response&& r, IdType txid);
		void processTxRequest(IdType txid);

		std::optional<IdType> init(string&& tx_hash);

	public:
		BtcTransactionTracer() = delete;
		pair<std::vector<AddressType>, std::vector<IdType>> traceAddresses(string txHash);
		BtcTransactionTracer(const TracerConfig& conf);
		~BtcTransactionTracer();
	};
}