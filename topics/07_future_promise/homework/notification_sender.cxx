#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <libmail/include/randomize_results.h>
#include <libmail/include/send_mail.h>

// libmail
// void sendMail(std::promise<bool>&& sendPromise, const std::string& address, const std::string& message)
// {
//     bool result = libmail::send_mail(address, message);
//     sendPromise.set_value(result);
// }

void sendMail(std::promise<bool>&& sendPromise, const std::string& address, const std::string& message)
{
    try {
        libmail::send_mail(address, message);
        sendPromise.set_value(true);
    } catch (...) {
        sendPromise.set_exception(std::current_exception());
    }
}

int main()
{
    auto startTime = std::chrono::system_clock::now();
    libmail::randomize_results();
    std::vector<std::string> responsible_devs = {
        "dev1@company.com",
        "dev2@company.com",
        "dev3@company.com",
        "dev4@company.com",
        "dev5@company.com",
    };
    std::string message = "build failed";

    std::vector<std::thread> threads;
    std::vector<std::promise<bool>> promises(responsible_devs.size());
    std::vector<std::future<bool>> futures;

    for (size_t i = 0; i < responsible_devs.size(); ++i) {
        futures.emplace_back(promises[i].get_future());
        threads.emplace_back(sendMail, std::move(promises[i]), std::cref(responsible_devs[i]), std::cref(message));
		threads[i].detach();
    }

    for (size_t i = 0; i < responsible_devs.size(); ++i) {
        try {
            futures[i].get();
            std::cout << "Sending mail to: " << responsible_devs[i] << " - OK\n";
        } catch (const std::exception& err) {
            std::cout << "Sending mail to: " << responsible_devs[i] << " failed - " << err.what() << '\n';
        }
        // std::cout << "Sending mail to: " << responsible_devs[i] << " - " << futures[i].get() << '\n'; //libmail
    }

    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    std::cout << "Elapsed time" << diff.count() << " s\n";
}
