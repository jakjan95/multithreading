#include <future>
#include <iostream>
using namespace std;

void f()
{
    this_thread::sleep_for(1s);
}

int main()
{
    auto fut = async(std::launch::async, f);

    if (fut.wait_for(0s) == future_status::deferred) {
        std::cout << "Sccheduled as deffered. Calling wait() to enforce exectuion\n";
        fut.wait();
    } else {
        while (fut.wait_for(100ms) == future_status::timeout) {
            cout << "Waiting...\n";
        }
        cout << "Finally...\n";
    }
}
