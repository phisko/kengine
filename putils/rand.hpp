#pragma once

#include <random>
#include <limits>
#include "static_if.hpp"

namespace putils
{
    // Random int generator
    template<typename T, T start = 0, T end = std::numeric_limits<T>::max>
    inline T rand() noexcept
    {
        T _; // used as parameter for static_if

        return static_if(std::is_floating_point<T>::value)
                .then([](auto)
                {
                    static std::random_device r;
                    static std::default_random_engine e(r());
                    std::uniform_real_distribution<T> d(start, end);
                    return d(e);
                })
                .else_([](auto)
                {
                    static std::random_device r;
                    static std::default_random_engine e(r());
                    static std::uniform_int_distribution<int> d;
                    return start + d(e) % (end - start);
                })(_);
    }
}
