#include <iterator>
#include <iostream>
#include <string>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>

#include "tracer/BtcTransactionTracer.h"

using btc_explorer::BtcTransactionTracer;
using btc_explorer::TracerConfig;


int main(int argc, char** argv) {
	static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::info, &consoleAppender);

	if (argc == 1) {
		PLOGF << "Up to 4 Additional arguments expected - tx_hash, out_file_path, threads_count, max_search_depth";
		return -1;
	}

	TracerConfig conf;
	conf.out_file = argc > 2 ? argv[2] : "out.txt";
	conf.threads_cnt = argc > 3 ? std::stoi(argv[3]) : 6;
	conf.max_depth = argc > 4 ? std::stoi(argv[4]) : 10;
	BtcTransactionTracer tracer(conf);

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