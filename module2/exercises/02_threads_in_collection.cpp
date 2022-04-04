#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>

using namespace std::chrono_literals;
// Secure your code so that each thread can safely enter its full text into the stream.

/*
Solution 1 using global mutex, not safe
std::mutex m;
void do_work(int id) {
    this_thread::sleep_for(100ms);
    m.lock();
    cout << "Thread [" << id << "]: " << "Job done!" << endl;
    m.unlock();
}
*/

//Solution 2 using RAII lock_gurad
void do_work(int id, std::mutex& m)
{
    std::this_thread::sleep_for(100ms);
    std::lock_guard<std::mutex> lock(m);
    std::cout << "Thread [" << id << "]: "
              << "Job done!" << std::endl;
}

/*Solution 3 fastes:
void do_work(int id)
{
    std::this_thread::sleep_for(100ms);
    std::stringstream ss;
    ss << "Thread [" << id << "]: "
       << "Job done!" << std::endl;
    std::cout << ss.rdbuf();
}
*/

int main() {
    std::mutex m;
    std::vector<std::thread> threads;
    for (int i = 0; i < 20; i++) {
        threads.emplace_back(std::thread(do_work, i, std::ref(m)));
    }
    for (auto && t : threads) {
        t.join();
    }
    return 0;
}
