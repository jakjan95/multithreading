#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;

std::atomic<bool> isDine = true;

struct Fork {
    bool busy { false };
    std::mutex mtx;
};

std::array<Fork, 5> forks;

int getRandom(int from, int to)
{
    std::mt19937 gen(std::random_device {}());
    std::uniform_int_distribution<int> uniformDist(from, to);
    return uniformDist(gen);
}

auto getRandomTime()
{
    return std::chrono::milliseconds(getRandom(100, 900));
}

void pickUpFork(int i)
{
    while (true) {
        if (forks[i].mtx.try_lock()) {
            forks[i].busy = true;
            return;
        } else {
            std::this_thread::sleep_for(getRandomTime());
        }
    }
}

void putDownFork(int i)
{
    forks[i].busy = false;
    forks[i].mtx.unlock();
}

void think(int i)
{
    std::stringstream ss;
    ss << "Philosopher " << i << " is thinking\n";
    std::cout << ss.rdbuf();
    std::this_thread::sleep_for(getRandomTime());
}

void eat(int i)
{
    std::stringstream ss;
    ss << "Philosopher " << i << " \t\tstarts eating\n";
    std::cout << ss.rdbuf();
    std::this_thread::sleep_for(getRandomTime());
    ss << "Philosopher " << i << " \t\t\t\tfinished eating\n";
    std::cout << ss.rdbuf();
}

void philosopher(int i)
{
    while (isDine) {
        think(i);

        pickUpFork(i);
        std::this_thread::sleep_for(getRandomTime());

        pickUpFork((i + 1) % 5);
        std::this_thread::sleep_for(getRandomTime());

        eat(i);

        putDownFork(i);
        putDownFork((i + 1) % 5);
    }
}

void dine()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Dine started!\n";

    std::thread t1(philosopher, 1);
    std::thread t2(philosopher, 2);
    std::thread t3(philosopher, 3);
    std::thread t4(philosopher, 4);
    std::thread t5(philosopher, 5);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    isDine = false;

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
}

int main()
{
    dine();
    return 0;
}
