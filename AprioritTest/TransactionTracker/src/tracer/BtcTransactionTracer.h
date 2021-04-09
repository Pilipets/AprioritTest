#pragma once

#include "oatpp/core/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

class BtcApiClient;

class BtcTransactionTracer {
	std::shared_ptr<BtcApiClient> explorerApi;
public:
	BtcTransactionTracer(std::shared_ptr<BtcApiClient> explorerApi);
	std::vector<std::string> traceFunds(oatpp::String &&txid);
};