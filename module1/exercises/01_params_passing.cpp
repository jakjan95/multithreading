#include <thread>
#include <iostream>

int add(int a, int b)
{
    return a + b;
}

int main()
{
    std::thread t(add, 3,4);
    t.join();
    return 0;
}

