
#include "client/BtcApiClient.h"
#include "AppComponent.h"
#include "tracer/BtcTransactionTracer.h"

#include <iostream>
#include <iterator>
#include <cassert>

typedef BtcTransactionTracer TxTracer;

void run(std::string txid) {
	AppConfig config{ "blockchain.info", 443, 5 };
	AppComponent components(config);

	OATPP_COMPONENT(std::shared_ptr<BtcApiClient>, btcApiClient);
	TxTracer tracer(btcApiClient);

	auto result = tracer.traceFunds(std::move(txid.c_str()));
	copy(result.begin(), result.end(), std::ostream_iterator<std::string>(std::cout, ", "));
}

int main(int argc, char *argv[]) {
	if (argc != 2)
		throw std::logic_error("First transaction as an argument ':txhash' is expected");

	oatpp::base::Environment::init();
	run(argv[1]);
	oatpp::base::Environment::destroy();
}