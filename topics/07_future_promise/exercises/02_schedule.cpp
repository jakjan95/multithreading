#include <exception>
#include <functional>
#include <future>
#include <thread>

int get_number()
{
    return 10;
}

std::future<int> schedule(std::function<int()> func)
{
    std::promise<int> prom;
    auto fut = prom.get_future();

    auto wrappedFun = [func](std::promise<int> p) {
        try {
            p.set_value(func());

        } catch (...) {
            p.set_exception(std::current_exception());
        }
    };
    std::thread t(wrappedFun, std::move(prom));
    t.detach();
    return fut;
}

int main()
{
    auto future = schedule(get_number);
    return future.get();
    // TODO: uncomment after implementation
}
