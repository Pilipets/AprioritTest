#pragma once

#include <string>
#include <functional>

#include <cpr/response.h>

namespace btc_explorer {
	using std::string;

	class BtcApiClient {
		const string host = "https://blockchain.info/";
	public:
		cpr::Response getTxRaw(string tx_hash);
		cpr::Response getTxRaw(uint64_t txid);
	};
}