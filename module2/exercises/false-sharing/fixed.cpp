
#include <atomic>
#include <iostream>
#include <thread>

void calculateNumbers(int upTo, std::atomic<int>& result)
{
    for (int i = 1; i < upTo; ++i) {
        result += i;
    }
}

struct alignas(64) Value {
    std::atomic<int> val_ { 0 };
};

void performOperations()
{
    constexpr int calculateUpTo = 10000;

    Value val1;
    Value val2;
    Value val3;
    Value val4;

    std::thread t1(calculateNumbers, calculateUpTo, std::ref(val1.val_));
    std::thread t2(calculateNumbers, calculateUpTo, std::ref(val2.val_));
    std::thread t3(calculateNumbers, calculateUpTo, std::ref(val3.val_));
    std::thread t4(calculateNumbers, calculateUpTo, std::ref(val4.val_));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    const auto result = val1.val_ + val2.val_ + val3.val_ + val4.val_;
    std::cout << "result = " << result << '\n';
}

int main()
{
    performOperations();
    return 0;
}
