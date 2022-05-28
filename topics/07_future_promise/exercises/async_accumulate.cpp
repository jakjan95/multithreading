#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <numeric>
#include <vector>

template <typename It, typename T>
T asyncAccumulate(It first, It last, T init)
{
    if (first == last) {
        return init;
    }

    const size_t hardwareThreads = std::thread::hardware_concurrency();
    std::vector<std::future<T>> futures(hardwareThreads);
    const auto numOfDataInBlock = std::distance(first, last) / hardwareThreads;

    auto accumulateFragment = [](It begin, It end) {
        return std::accumulate(begin, end, T {});
    };

    auto blockBegin = first;
    auto blockEnd = std::next(first, numOfDataInBlock);
    for (size_t i = 0; i < hardwareThreads; ++i) {
        futures[i] = std::async(std::launch::async, accumulateFragment, blockBegin, blockEnd);
        blockBegin = blockEnd;
        std::advance(blockEnd, numOfDataInBlock);
    }
    auto lastResults = accumulateFragment(blockBegin, last);

    return std::accumulate(std::begin(futures), std::end(futures), init + lastResults, [](auto result, auto& fut) {
        return std::move(result) + fut.get();
    });
}

int main()
{
    std::vector<int> vec(1'000'000);
    std::generate(begin(vec), end(vec), [x { 0 }]() mutable { return ++x; });
    auto start = std::chrono::steady_clock::now();
    auto result1 = asyncAccumulate(begin(vec), end(vec), 0);
    auto stop = std::chrono::steady_clock::now();

    auto start2 = std::chrono::steady_clock::now();
    auto result2 = std::accumulate(std::begin(vec), std::end(vec), 0);
    auto stop2 = std::chrono::steady_clock::now();

    std::cout << "\nAsync algorithm: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
              << "us" << std::endl;
    std::cout << "\nNormal algorithm: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2).count()
              << "us" << std::endl;
    std::cout << '\n';

    std::cout << "parallelAccumulate == " << result1 << '\n';
    std::cout << "std::accumulate == " << result2 << '\n';

    return 0;
}
