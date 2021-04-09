#include "BtcApiClient.h"

BtcApiClient::BtcApiClient(std::shared_ptr<oatpp::data::mapping::ObjectMapper> objectMapper,
	std::shared_ptr<oatpp::web::client::RequestExecutor> requestExecutor)
	: oatpp::web::client::ApiClient(requestExecutor, objectMapper) {
}