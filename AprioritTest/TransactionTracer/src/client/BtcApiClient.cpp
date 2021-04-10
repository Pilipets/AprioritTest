#include "BtcApiClient.h"

#include <cpr/cpr.h>

namespace btc_explorer {

	std::future<void> BtcApiClient::getTxRawAsync(uint64_t txid, std::function<void(cpr::Response)>&& cb) {
		string url = host + "rawtx/" + std::to_string(txid);
		return cpr::GetCallback(std::move(cb), cpr::Url{ std::move(url) });
	}

	cpr::Response BtcApiClient::getTxRaw(string tx_hash) {
		string url = host + "rawtx/" + tx_hash;
		return cpr::Get(cpr::Url{ std::move(url) });
	}

}

