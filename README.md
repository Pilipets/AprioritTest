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
5. BtcTransactionTracer::tx_cache stores the transaction where unspent outputs were found - tx_cache.size() <= res.size().
Besides additional unspent-related details, it serves as a cache, to avoid processing the same transaction multiple times.
5. UPDATED - tx_cache stores all the processed transactions, it increased the performance.

PAY ATTENTION:
1. There is an enormous drawback with thread pool processing logic - we need a cache to avoid processing the same transaction twice - nonetheless, a loop is impossible.
2. We can eliminate the necessity of storing the cache using the transaction index sorting technique and one thread worker.
3. One-threaded version is available in developer branch.
3. I haven't tested which approach is faster yet.