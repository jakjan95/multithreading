#pragma once
#include "Task.hpp"
#include "ThreadSafeQueue.hpp"
#include <array>
#include <cstddef>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <thread>

class ThreadPool {
    std::vector<std::thread> threads;
    ThreadSafeQueue<Task> taskQueue;
public:
    ThreadPool(const unsigned size)
    {
        auto job = [&]() {
            while (true) {
                auto&& task = taskQueue.pop();
                if (!task.algo) {
                    return;
                }
                task.algo(task.input, task.output);
                task.promise.set_value({ task.id, task.output });
            }
        };
        threads.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            threads.emplace_back(job);
        }
    }

    ~ThreadPool()
    {
        for (size_t i = 0; i < threads.size(); ++i) {
            taskQueue.push(Task {});
        }

        for (auto&& t : threads) {
            t.join();
        }
    }

    auto enqueue(Task&& task)
    {
        if (task.algo) {
            auto fut = task.promise.get_future();
            taskQueue.push(std::move(task));
            return fut;
        }

        std::promise<Task::PromiseType> p;
        p.set_exception(std::make_exception_ptr(std::logic_error { "Quit task!" }));
        return p.get_future();
    }
};
