#include <cstddef>
#include <iostream>
#include <iterator>
#include <thread>
#include <numeric>
#include <algorithm>
#include <vector>
#include <chrono>
#include <functional>
#include <execution>

template <typename IT, typename UnaryPredicate>
int parallelCountIf(IT first, IT last, UnaryPredicate pred)
{
    int hardwareThreads = std::thread::hardware_concurrency();
    std::vector<int> results(hardwareThreads);
    std::vector<std::thread> threads(hardwareThreads - 1);
    auto numOfDataInBlock = std::distance(first, last) / hardwareThreads;

    auto count = [](IT begin, IT end, UnaryPredicate p, int& result) {
        result = std::count_if(begin, end, p);
    };

    auto blockBegin = first;
    auto blockEnd = std::next(first, numOfDataInBlock);
    for (int i = 0; i < threads.size(); i++) {
        threads[i] = std::thread(count, blockBegin, blockEnd, pred, std::ref(results[i]));
        std::advance(blockBegin, numOfDataInBlock);
        std::advance(blockEnd, numOfDataInBlock);
    }

    count(blockBegin, blockEnd, pred, std::ref(results.back()));

    for (auto&& t : threads) {
        t.join();
    }

    return std::accumulate(results.cbegin(), results.cend(), 0);
}

int main() {
    auto isDisivableBy4 = [](int num) { return num % 4 == 0; };

    std::vector<int> vec(1'000'000);
    std::generate(begin(vec), end(vec), [x{0}]() mutable { return ++x; });
    auto start = std::chrono::steady_clock::now();
    auto result1 = parallelCountIf(std::begin(vec), std::end(vec), isDisivableBy4);
    auto stop = std::chrono::steady_clock::now();

    auto start2 = std::chrono::steady_clock::now();
    auto result2 = std::count_if(std::begin(vec), std::end(vec), isDisivableBy4);
    auto stop2 = std::chrono::steady_clock::now();

    auto start3 = std::chrono::steady_clock::now();
    auto result3 = std::count_if(std::execution::par ,vec.begin(), vec.end(), isDisivableBy4);
    auto stop3 = std::chrono::steady_clock::now();

    std::cout << "\nParallel algorithm: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
              << "us" << std::endl;

    std::cout << "Normal algorithm: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2).count()
              << "us" << std::endl;

    std::cout << "Normal algorithm with execution policy: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop3 - start3).count()
              << "us" << std::endl;
    
    std::cout << '\n';
    std::cout << "parallelAccumulate == " << result1 << '\n';
    std::cout << "std::accumulate == " << result2 << '\n';
    std::cout << "std::accumulate(par == " << result3 << '\n';

    return 0;
}
