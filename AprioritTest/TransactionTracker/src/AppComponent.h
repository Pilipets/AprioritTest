#pragma once

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/tcp/client/ConnectionProvider.hpp"

#include "oatpp/core/macro/component.hpp"


struct AppConfig {
    const char* btc_host;
    uint16_t btc_port;
    uint8_t async_queries;
};

class BtcApiClient;

class AppComponent {
private:
    std::shared_ptr<BtcApiClient> createBtcApiClient(const char *host, uint16_t port, uint8_t async_count);
public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
        OATPP_LOGI("ObjectMapper", "Using default object mapper");
        return oatpp::parser::json::mapping::ObjectMapper::createShared();
    }());

    oatpp::base::Environment::Component<std::shared_ptr<BtcApiClient>> btcApiClient;

    AppComponent(const AppConfig& config);
};