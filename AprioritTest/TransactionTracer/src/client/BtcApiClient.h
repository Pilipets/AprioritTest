#pragma once

#include <cpr/response.h>
#include <future>
#include <string>
#include <functional>

using std::string;

class BtcApiClient {
	const string host = "https://blockchain.info/";
public:
	std::future<void> getTxRawAsync(const string &txid, std::function<void (cpr::Response)>&& cb);
};