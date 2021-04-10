Comments for the project.

Dependencies:
1. C++17
2. https://github.com/SergiusTheBest/plog
3. https://github.com/nlohmann/json
4. https://github.com/whoshuu/cpr
5. Compiled Debug x64 with VisualStudio/16.9.3+31129.286.

Details:
1. Stack: C++14, oat++
2. Console logging.
3. Accepts one argument(transaction hash) from the command line.
4. Compiled static libraries - JSON, cpr - can be added upon request.
5. Some transactions network requests may fail for whatever reason - we can't repeat them again and again since it may lead to the infinity loop,
so we store them into the BtcTransactionTracer::tx_err.
6. BtcTransactionTracer::tx_cache stores the transaction where unspent outputs were found - tx_cache.size() <= res.size().
Besides additional unspent-related details, it serves as a cache, to avoid processing the same transaction multiple times.