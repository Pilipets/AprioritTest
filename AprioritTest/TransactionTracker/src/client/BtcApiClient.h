#pragma once

#include "oatpp/web/client/ApiClient.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiClient)

class BtcApiClient : public oatpp::web::client::ApiClient {
	API_CLIENT_INIT(BtcApiClient)
public:
	BtcApiClient(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper,
		std::shared_ptr<oatpp::web::client::RequestExecutor> requestExecutor);

	API_CALL("GET", "rawtx/{txid}", getRawTx, PATH(oatpp::String, txid, "txid"))
};

#include OATPP_CODEGEN_END(ApiClient)