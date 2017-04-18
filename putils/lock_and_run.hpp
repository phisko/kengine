#pragma once

#include <mutex>

namespace putils
{
    // Lock a mutex, execute a functor, unlock mutex
    template<typename Func, typename ...Args>
    std::result_of_t<Func(Args...)> lock_and_run(std::mutex &m, Func &&f, Args &&...args)
    {
        std::lock_guard<std::mutex> l(m);
        return f(std::forward<Args>(args)...);
    }
}