#include <iostream>
#include <iterator>
#include <thread>
#include <numeric>
#include <algorithm>
#include <vector>
#include <chrono>
#include <functional>

template <typename It, typename T>
T parallelAccumulate(It first, It last, T init)
{
    if (first == last) {
        return init;
    }
    int hardwareThreads = std::thread::hardware_concurrency();

    std::vector<int> results(hardwareThreads);
    std::vector<std::thread> threads(hardwareThreads - 1);
    auto numOfDataInBlock = std::distance(first, last) / hardwareThreads;

    auto accumulate = [](It begin, It end, T& result) {
        result = std::accumulate(begin, end, T {});
    };

    auto blockBegin = first;
    auto blockEnd = std::next(first, numOfDataInBlock);
    for (int i = 0; i < threads.size(); i++) {
        threads[i] = std::thread(accumulate, blockBegin, blockEnd, std::ref(results[i]));
        std::advance(blockBegin, numOfDataInBlock);
        std::advance(blockEnd, numOfDataInBlock);
    }

    accumulate(blockBegin, blockEnd, results.back());

    for (auto&& t : threads) {
        t.join();
    }

    return std::accumulate(results.cbegin(), results.cend(), init);
}

int main() {
    std::vector<int> vec(1'000'000);
    std::generate(begin(vec), end(vec), [x{0}]() mutable { return ++x; });
    auto start = std::chrono::steady_clock::now();
    auto result1 = parallelAccumulate(std::begin(vec), std::end(vec), 0);
    auto stop = std::chrono::steady_clock::now();

    auto start2 = std::chrono::steady_clock::now();
    auto result2 = std::accumulate(std::begin(vec), std::end(vec), 0);
    auto stop2 = std::chrono::steady_clock::now();

    std::cout << "\nParallel algorithm: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
              << "us" << std::endl;
    std::cout << "\nNormal algorithm: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2).count()
              << "us" << std::endl;
    std::cout<<'\n';
    std::cout << "parallelAccumulate == " << result1 << '\n';
    std::cout << "std::accumulate == " << result2 << '\n';

    return 0;
}
