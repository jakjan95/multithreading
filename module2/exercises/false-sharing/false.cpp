
#include <atomic>
#include <iostream>
#include <thread>

void calculateNumbers(int upTo, std::atomic<int>& result)
{
    for (int i = 1; i < upTo; ++i) {
        result += i;
    }
}

void performOperations()
{
    constexpr int calculateUpTo = 10000;

    std::atomic<int> val1 { 0 };
    std::atomic<int> val2 { 0 };
    std::atomic<int> val3 { 0 };
    std::atomic<int> val4 { 0 };

    std::thread t1(calculateNumbers, calculateUpTo, std::ref(val1));
    std::thread t2(calculateNumbers, calculateUpTo, std::ref(val2));
    std::thread t3(calculateNumbers, calculateUpTo, std::ref(val3));
    std::thread t4(calculateNumbers, calculateUpTo, std::ref(val4));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    const auto result = val1 + val2 + val3 + val4;
    std::cout << "result = " << result << '\n';
}

int main()
{
    performOperations();
    return 0;
}
