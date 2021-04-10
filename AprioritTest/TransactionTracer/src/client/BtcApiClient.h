#pragma once

#include <cpr/response.h>
#include <cpr/session.h>
#include <future>
#include <string>
#include <functional>
using std::string;

class BtcApiClient {
	const string host = "https://blockchain.info/";
public:
	std::future<void> getTxRawAsync(uint64_t txid, std::function<void (cpr::Response)>&& cb);
	cpr::Response getTxRaw(string tx_hash);
};