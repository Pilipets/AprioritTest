#include <stdexcept>
#include <iterator>
#include <iostream>
#include <string>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>

#include "tracer/BtcTransactionTracer.h"

#include <boost/asio/thread_pool.hpp>

using btc_explorer::BtcTransactionTracer;
using btc_explorer::TracerConfig;


int main(int argc, char** argv) {
	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::info, &consoleAppender);

	if (argc != 2) {
		PLOGD << "First transaction as an argument 'tx_hash' is expected";
		throw std::logic_error("First transaction as an argument ':txhash' is expected");
	}

	BtcTransactionTracer tracer(TracerConfig{ 6, "out.txt" });

	try {
		auto [res, err] = tracer.traceAddresses(argv[1]);

		std::cout << "Traced " << res.size() << " addresses:";
		copy(res.begin(), res.end(), std::ostream_iterator<std::string>(std::cout, ", "));
		std::cout << "\n\n";

		if (err.size()) {
			std::cout << "Unable to trace" << err.size() << " transactions:";
			copy(err.begin(), err.end(), std::ostream_iterator<uint64_t>(std::cout, ", "));
			std::cout << "\n\n";
		}
	}
	catch (std::exception& ex) {
		PLOGF << "Unhandled exception encountered during tracing: " << ex.what() << "\n";
	}
	return 0;
}