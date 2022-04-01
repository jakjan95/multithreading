#include <thread>
#include <iostream>

void add10(int & a)
{
    a += 10;
}

int main()
{
    int value = 5;
    std::cout << "value=" << value << '\n';
    std::thread t(add10, std::ref(value));
    std::cout << "value=" << value << '\n';
    t.join();
    std::cout << "value=" << value << '\n';
    return 0;
}
