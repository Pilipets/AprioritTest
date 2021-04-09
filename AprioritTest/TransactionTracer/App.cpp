#include <stdexcept>
#include <iterator>
#include <iostream>

#include "tracer/BtcTransactionTracer.h"


int main(int argc, char** argv) {
	if (argc != 2)
		throw std::logic_error("First transaction as an argument ':txhash' is expected");
	typedef BtcTransactionTracer TxTracer;

	TxTracer tracer(TracerConfig{ 5 });

	auto res = tracer.traceAddresses(argv[1]);

	std::cout << "Traced addresses: ";
	copy(res.begin(), res.end(), std::ostream_iterator<string>(std::cout, ", "));
	std::cout << "\n\n";
}