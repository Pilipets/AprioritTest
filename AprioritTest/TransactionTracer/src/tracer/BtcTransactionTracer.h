#pragma once
#include <memory>
#include <queue>
#include <string>
#include <mutex>
#include <vector>
#include <unordered_set>
#include <utility>
#include <cpr/response.h>

using std::string;

struct TracerConfig {
	uint8_t async_cnt = 5;
};

#include "client/BtcApiClient.h"

class BtcTransactionTracer {
	typedef BtcApiClient BtcApi;
	typedef uint64_t IdType;
	typedef string AddressType;

	std::unique_ptr<BtcApi> btcApi;
	const uint8_t async_cnt;

	std::mutex mx;
	std::queue<IdType> q;
	std::unordered_set<IdType> cache;
	std::unordered_set<IdType> err_cache;

	std::unordered_set<AddressType> res;

	IdType getNextAddress();
	void processTxResponse(cpr::Response &&r, IdType txid);

	bool init(string &&tx_hash);
public:
	BtcTransactionTracer() = delete;
	std::vector<string> traceAddresses(string txHash);
	BtcTransactionTracer(const TracerConfig &conf);
};