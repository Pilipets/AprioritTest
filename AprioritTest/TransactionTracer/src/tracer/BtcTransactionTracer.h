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

class BtcApiClient;

class BtcTransactionTracer {
	typedef BtcApiClient BtcApi;
	std::shared_ptr<BtcApi> btcApi;
	const uint8_t async_cnt;

	std::mutex mx;
	std::queue<string> q;
	std::unordered_set<string> res;		//res addresses
	std::unordered_set<string> err_res; //error addresses

	string getNextAddress();
	void processTxResponse(cpr::Response &&r, string txid);
public:
	BtcTransactionTracer() = delete;
	std::pair<std::vector<string>, std::vector<string>> traceAddresses(string txid);
	BtcTransactionTracer(std::shared_ptr<BtcApi> btcApi, const TracerConfig &conf);
};