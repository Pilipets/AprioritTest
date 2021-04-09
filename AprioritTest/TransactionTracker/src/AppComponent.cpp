#include "AppComponent.h"

#include "client/BtcApiClient.h"

std::shared_ptr<BtcApiClient> AppComponent::createBtcApiClient(const char* host, uint16_t port, uint8_t async_count)
{
    OATPP_LOGI("BtcApiClient", "Using btc explorer api='%s:%d'", host, port);

    auto connectionProvider = oatpp::network::tcp::client::ConnectionProvider::createShared({ host, port, oatpp::network::Address::IP_4 });
    auto requestExecutor = oatpp::web::client::HttpRequestExecutor::createShared(connectionProvider);

    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);
    return BtcApiClient::createShared(requestExecutor, objectMapper);
}

AppComponent::AppComponent(const AppConfig& config):
    btcApiClient(createBtcApiClient(config.btc_host, config.btc_port, config.async_queries)) {
}
