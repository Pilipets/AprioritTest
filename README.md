Comments for the project.

Dependencies:
1. C++17
2. https://github.com/SergiusTheBest/plog
3. https://github.com/nlohmann/json
4. https://github.com/whoshuu/cpr
5. boost::asio
5. Compiled Debug x64 with VisualStudio/16.9.3+31129.286.
6. On Windows run .exe from AprioritTest/x64/Debug.

Details:
1. Console logging.
2. Up to 4 Additional arguments expected - tx_hash, out_file_path, threads_count, max_search_depth.
3. Compiled static libraries - JSON, cpr - can be added upon request.
4. Some transactions network requests may fail for whatever reason - we can't repeat them again and again since it may lead to the infinity loop,
so we store them into the BtcTransactionTracer::tx_err.
5. UPDATED - tx_cache stores is lru of all the processed transactions.

PAY ATTENTION:
1. There is an enormous drawback with thread pool processing logic - we need a cache to speed up the proess by skipping already processed transactions - nonetheless, a loop is impossible.
2. We can eliminate the necessity of storing the cache using the transaction index sorting technique and one thread worker.
3. One-threaded version is available in developer branch.