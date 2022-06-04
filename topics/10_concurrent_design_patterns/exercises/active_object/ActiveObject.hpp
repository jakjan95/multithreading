#pragma once
#include "Object.hpp"
#include <memory>
#include <future>
#include <mutex>
#include <iostream>
#include <thread>

class ActiveObject
{
    std::unique_ptr<Object> object_;
    std::mutex m_;

public:
    ActiveObject(std::unique_ptr<Object> obj)
        : object_(std::move(obj))
    {}

    std::future<void> push([[maybe_unused]] const int value)
    {
        auto pushFun = [&](const int val) {
            std::lock_guard lg(m_);
            std::cout << std::this_thread::get_id() << '\n';
            return object_->push(val);
        };
        return std::async(std::launch::async, pushFun, value);
    }

    std::future<int> pop()
    {
        auto popFun = [&] {
            std::lock_guard lg(m_);
            std::cout << std::this_thread::get_id() << '\n';
            return object_->pop();
        };
        return std::async(std::launch::async, popFun);
    }
};
