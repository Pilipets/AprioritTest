#include <stdexcept>
#include <iterator>
#include <iostream>

#include "tracer/BtcTransactionTracer.h"
#include "client/BtcApiClient.h"


int main(int argc, char** argv) {
	if (argc != 2)
		throw std::logic_error("First transaction as an argument ':txhash' is expected");
	typedef BtcTransactionTracer TxTracer;

	auto api = std::make_shared<BtcApiClient>();
	auto config = TracerConfig{ 5 };
	TxTracer tracer(api, config);

	auto [res, err_res] = tracer.traceAddresses(argv[1]);

	std::cout << "Traced addresses: ";
	copy(res.begin(), res.end(), std::ostream_iterator<string>(std::cout, ", "));
	std::cout << "\n\n";

	std::cout << "Error processing addresses: ";
	copy(err_res.begin(), err_res.end(), std::ostream_iterator<string>(std::cout, ", "));
	std::cout << "\n";
}