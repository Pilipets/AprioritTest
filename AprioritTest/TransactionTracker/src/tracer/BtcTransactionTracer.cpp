#include "BtcTransactionTracer.h"

#include "client/BtcApiClient.h"
BtcTransactionTracer::BtcTransactionTracer(std::shared_ptr<BtcApiClient> explorerApi):
	explorerApi(explorerApi)
{
}

std::vector<std::string> BtcTransactionTracer::traceFunds(oatpp::String &&txid)
{
	OATPP_LOGD("BtcTransactionTracer", "Trasing funds for txid='%s'", txid->c_str());

	auto response = explorerApi->getRawTx(std::move(txid));
	OATPP_LOGD("UpdateOrderCoroutine", "[traceFunds] code=%d, msg='%s'",
		response->getStatusCode(), response->getStatusDescription()->c_str());
	return std::vector<std::string>();
}
