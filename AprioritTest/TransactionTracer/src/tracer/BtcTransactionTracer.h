#pragma once
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <unordered_set>
#include <queue>

#include <cpr/response.h>

#include "client/BtcApiClient.h"

namespace btc_explorer {
	using std::pair;
	using std::string;

	struct TracerConfig {
		std::uint8_t threads_cnt;
		std::string out_file;
		size_t max_depth;
	};


	class BtcTransactionTracer {
		typedef BtcApiClient BtcApi;
		typedef uint64_t IdType;
		typedef string AddressType;

		const std::unique_ptr<BtcApi> btcApi;

		std::priority_queue<pair<IdType, size_t>,
			std::vector<pair<IdType, size_t>>,
			std::greater<pair<IdType, size_t>>> q;

		std::unordered_set<IdType> tx_err;
		std::unordered_set<AddressType> res;

		TracerConfig conf;
	private:

		void processTxResponse(cpr::Response&& r, IdType txid, size_t depth);
		void processTxRequest(IdType txid, size_t depth);

		std::optional<IdType> init(string&& tx_hash);

	public:
		BtcTransactionTracer() = delete;
		pair<std::vector<AddressType>, std::vector<IdType>> traceAddresses(string txHash);
		BtcTransactionTracer(const TracerConfig& conf);
	};
}