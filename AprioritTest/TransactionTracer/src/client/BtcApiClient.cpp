#include "BtcApiClient.h"

#include <cpr/cpr.h>

std::future<void> BtcApiClient::getTxRawAsync(const string &txid, std::function<void(cpr::Response)> &&cb) {
	string url = host + "rawtx/" + txid;
	return cpr::GetCallback(std::move(cb), cpr::Url{ url });
}

