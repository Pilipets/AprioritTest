#pragma once

#include <future>
#include <string>
#include <functional>

#include <cpr/response.h>

namespace btc_explorer {
	using std::string;

	class BtcApiClient {
		const string host = "https://blockchain.info/";
	public:
		std::future<void> getTxRawAsync(uint64_t txid, std::function<void(cpr::Response)>&& cb);
		cpr::Response getTxRaw(string tx_hash);
	};
}