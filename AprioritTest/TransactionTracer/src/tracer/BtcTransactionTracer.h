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
#include "client/BtcApiClient.h"

namespace btc_explorer {
	using std::pair;
	using std::string;

	struct TracerConfig {
		uint8_t async_cnt = 5;
	};


	class BtcTransactionTracer {
		typedef BtcApiClient BtcApi;
		typedef uint64_t IdType;
		typedef string AddressType;

		const std::unique_ptr<BtcApi> btcApi;
		const uint8_t async_cnt;

		std::mutex mx;
		std::queue<IdType> q;
		std::unordered_set<IdType> tx_cache;

		std::unordered_set<IdType> tx_err;
		std::unordered_set<AddressType> res;

		std::optional<IdType> getNextAddress();
		void processTxResponse(cpr::Response&& r, IdType txid);

		bool init(string&& tx_hash);
	public:
		BtcTransactionTracer() = delete;
		pair<std::vector<string>, std::vector<IdType>> traceAddresses(string txHash);
		BtcTransactionTracer(const TracerConfig& conf);
	};
}