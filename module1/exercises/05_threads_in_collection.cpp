#include <iostream>
#include <thread>
#include <vector>

void doAction(int id)
{
    std::cout << id << '\n';
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 20; i++) {
        threads.emplace_back(doAction, i);
    }

    for (auto&& t : threads) {
        t.join();
    }

    return 0;
}
