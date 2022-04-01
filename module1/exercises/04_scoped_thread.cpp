#include <ctime>
#include <thread>
#include <stdexcept>
#include <chrono>
#include <iostream>
// using namespace std;

using namespace std::chrono_literals; //for 1s

class scoped_thread {
public:
    explicit scoped_thread(std::thread t)
        : t_ { std::move(t) }
    {
        if (!t_.joinable()) {
            throw std::runtime_error("No thread!");
        }
    }

    scoped_thread(const scoped_thread&) = delete;
    scoped_thread& operator=(const scoped_thread&) = delete;

    scoped_thread(scoped_thread&&) = default;
    scoped_thread& operator=(scoped_thread&&) = default;

    ~scoped_thread()
    {
        if (t_.joinable()) {
            t_.join();
        }
    }

private:
    std::thread t_;
};

void do_sth(int) {
    std::this_thread::sleep_for(1s);
    std::cout << std::this_thread::get_id() << '\n';
}

void do_sth_unsafe_in_current_thread() {
    throw std::runtime_error("Whoa!");
}

int main() try {
    // TODO: Uncomment
    scoped_thread st(std::thread(do_sth, 42));
    // auto st2 = st; // copying not allowed
    [[maybe_unused]] auto st3 = std::move(st);
    // scoped_thread st4(do_sth, 42);
    do_sth_unsafe_in_current_thread();
    return 0;
} catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
    return -1;
} // thread is safely destroyed
