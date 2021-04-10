#include <stdexcept>
#include <iterator>
#include <iostream>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>

#include "tracer/BtcTransactionTracer.h"

int main(int argc, char** argv) {
	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::verbose, &consoleAppender);

	if (argc != 2) {
		PLOGD << "First transaction as an argument 'tx_hash' is expected";
		throw std::logic_error("First transaction as an argument ':txhash' is expected");
	}

	BtcTransactionTracer tracer(TracerConfig{ 5 });

	auto res = tracer.traceAddresses(argv[1]);

	PLOGI << "Traced " << res.size() << " addresses:";
	copy(res.begin(), res.end(), std::ostream_iterator<string>(std::cout, ", "));
	return 0;
}