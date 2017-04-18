#pragma once

#include <functional>

namespace pmeta
{
    // Implementation details
    namespace detail
    {
        template<typename F, typename Tuple, size_t ...Is>
        void tuple_for_each(F &&f, Tuple &&tuple, std::index_sequence<Is...>);

        template<typename F, typename Tuple>
        void tuple_for_each(F &&, Tuple &&, std::index_sequence<>) {}

        template<typename F, typename Tuple, size_t I, size_t ...Is>
        void tuple_for_each(F &&f, Tuple &&tuple, std::index_sequence<I, Is...>)
        {
            f(std::get<I>(tuple));
            detail::tuple_for_each(std::forward<F>(f), tuple, std::index_sequence<Is...>());
        }

        template<template<typename> typename Func>
        void for_each() {}

        template<template<typename> typename Func, typename T, typename ...Args>
        void for_each()
        {
            Func<T>::func();
            detail::for_each<Func, Args...>();
        }
    }

    // For each `e` element in `tuple`, call `f(e)`
    // For instance:
    //      std::tuple<int, std::string, int> t(42, "test", 1);
    //      pmeta::tuple_for_each(t, [](auto &attr) { std::cout << attr << std::endl; });
    // Will print
    //      42
    //      test
    //      1
    //
    template<typename F, typename ...Args>
    void tuple_for_each(std::tuple<Args...> &tuple, F &&f)
    {
        detail::tuple_for_each(std::forward<F>(f), tuple, std::index_sequence_for<Args...>());
    }

    template<typename F, typename ...Args>
    void tuple_for_each(const std::tuple<Args...> &tuple, F &&f)
    {
        detail::tuple_for_each(std::forward<F>(f), tuple, std::index_sequence_for<Args...>());
    }

    // For each type `T` in `Args`, call `Func<T>::func()`
    // Func: class with a static void func() function
    template<template<typename> typename Func, typename ...Args>
    void for_each()
    {
        detail::for_each<Func, Args...>();
    }

    namespace test
    {
        template<typename T>
        struct TestFunc
        {
            static void func()
            {
                // Do something with T
            }
        };

        inline void for_each()
        {
            pmeta::for_each<TestFunc, int, std::string>();
        }
    }
}
