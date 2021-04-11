#pragma once
#include <memory>
#include <string>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <utility>
#include <optional>
#include <fstream>

#include <cpr/response.h>
#include <boost/asio.hpp>
#include <boost/unordered_set.hpp>
#include <boost/compute/detail/lru_cache.hpp>

#include "client/BtcApiClient.h"

namespace btc_explorer {
	using std::pair;
	using std::string;

	struct TracerConfig {
		uint8_t threads_cnt;
		std::string out_file;
		size_t max_depth;
		size_t cache_size = 100000;
	};


	class BtcTransactionTracer {
		typedef BtcApiClient BtcApi;
		typedef uint64_t IdType;
		typedef string AddressType;

		const std::unique_ptr<BtcApi> btcApi;

		boost::asio::thread_pool pool;
		std::mutex cache_mx;
		boost::compute::detail::lru_cache<IdType, bool> tx_cache;

		std::mutex res_mx;
		boost::unordered_set<IdType> tx_err;
		boost::unordered_set<AddressType> res;

		TracerConfig conf;
		std::ofstream fout;
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