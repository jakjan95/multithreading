#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

class PingPong {
    int repetitions_;
    std::atomic<bool> play_ { true };
    bool isPingTurn_ { true };
    std::condition_variable cv_;
    std::mutex mtx_;

public:
    PingPong(int repetitions)
        : repetitions_(repetitions)
    {
    }

    void ping()
    {
        thread_local int localReps = 0;
        while (localReps < repetitions_ && play_) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&] { return isPingTurn_; });
            std::cout << "ping " << localReps << '\n';
            localReps++;
            isPingTurn_ = false;
            lock.unlock();
            this_thread::sleep_for(500ms);
        }

        if (localReps >= repetitions_) {
            std::cout << "Ping reached repetitions limit\n";
        }
    }

    void pong()
    {
        thread_local int localReps = 0;
        while (localReps < repetitions_ && play_) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&] { return !isPingTurn_; });
            std::cout << "pong " << localReps << '\n';
            localReps++;
            isPingTurn_ = true;
            lock.unlock();
            this_thread::sleep_for(500ms);
        }

        if (localReps >= repetitions_) {
            std::cout << "Pong reached timout limit\n";
            play_ = false;
            cv_.notify_all();
        }
    }

    void stop([[maybe_unused]] chrono::seconds timeout)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        //bug in wait_for causing double lock of a mutex
        auto isTimeout = cv_.wait_for(lock, timeout, [&] { return !play_; });
        if (!isTimeout) {
            play_ = false;
            std::cout << "Timeout\n";
        }
        cv_.notify_all();
    }
};

int main(int argc, char** argv)
{
    if (argc != 3) {
        return -1;
    }
    int repetitions = stoi(argv[1]);
    int timeout = stoi(argv[2]);
    PingPong pp(repetitions);
    thread t1(&PingPong::ping, &pp);
    thread t2(&PingPong::pong, &pp);
    thread t3(&PingPong::stop, &pp, chrono::seconds(timeout));
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
