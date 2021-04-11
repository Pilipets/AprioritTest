#include "BtcApiClient.h"

#include <cpr/cpr.h>

namespace btc_explorer {

	cpr::Response BtcApiClient::getTxRaw(string tx_hash) {
		string url = host + "rawtx/" + tx_hash;
		return cpr::Get(cpr::Url{ std::move(url) });
	}

	cpr::Response BtcApiClient::getTxRaw(uint64_t txid) {
		string url = host + "rawtx/" + std::to_string(txid);
		return cpr::Get(cpr::Url{ std::move(url) });
	}
}

