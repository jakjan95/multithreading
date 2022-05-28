#include <future>
#include <thread>

int get_number()
{
    return 10;
}

std::future<int> get_number_async()
{
    std::promise<int> num;
    auto f = num.get_future();

    auto fun = [](std::promise<int> p) {
        p.set_value(get_number());
    };
    std::thread t(fun, std::move(num));
    t.detach();
    return f;
}

int main()
{
    auto future = get_number_async();
    return future.get();
}
