// Copyright (c) 2016 Vittorio Romeo
// License: AFL 3.0 | https://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include "fwd.hpp"

template <bool TX>
using bool_ = std::integral_constant<bool, TX>;

template <bool TX>
constexpr bool_<TX> bool_v{};

template <int TX>
using int_ = std::integral_constant<int, TX>;

template <int TX>
constexpr int_<TX> int_v{};

template <std::size_t TX>
using sz_ = std::integral_constant<int, TX>;

template <std::size_t TX>
constexpr sz_<TX> sz_v{};

template <typename TPredicate>
auto static_if(TPredicate) noexcept;

namespace impl
{
    template <bool TPredicateResult>
        struct static_if_impl;

    template <typename TFunctionToCall>
        struct static_if_result;

    template <typename TF>
        auto make_static_if_result(TF&& f) noexcept;

    template <>
        struct static_if_impl<true>
        {
            template <typename TF>
                auto& else_(TF&&) noexcept
                {
                    // Ignore `else_`, as the predicate is true.
                    return *this;
                }

            template <typename TPredicate>
                auto& else_if(TPredicate) noexcept
                {
                    // Ignore `else_if`, as the predicate is true.
                    return *this;
                }

            template <typename TF>
                auto then(TF&& f) noexcept
                {
                    // We found a matching branch, just make a result and
                    // ignore everything else.
                    return make_static_if_result(FWD(f));
                }
        };

    template <>
        struct static_if_impl<false>
        {
            template <typename TF>
                auto& then(TF&&) noexcept
                {
                    // Ignore `then`, as the predicate is false.
                    return *this;
                }

            template <typename TF>
                auto else_(TF&& f) noexcept
                {
                    // (Assuming that `else_` is after all `else_if` calls.)

                    // We found a matching branch, just make a result and
                    // ignore everything else.

                    return make_static_if_result(FWD(f));
                }

            template <typename TPredicate>
                auto else_if(TPredicate) noexcept
                {
                    return static_if(TPredicate{});
                }

            template <typename... Ts>
                auto operator()(Ts&&...) noexcept
                {
                    // If there are no `else` branches, we must ignore calls
                    // to a failed `static_if` matching.
                }
        };

    template <typename TFunctionToCall>
        struct static_if_result : TFunctionToCall
    {
        // Perfect-forward the function in the result instance.
        template <typename TFFwd>
            static_if_result(TFFwd&& f) noexcept : TFunctionToCall(FWD(f))
            {
            }

        // Ignore everything, we found a result.
        template <typename TF>
            auto& then(TF&&) noexcept
            {
                return *this;
            }

        template <typename TPredicate>
            auto& else_if(TPredicate) noexcept
            {
                return *this;
            }

        template <typename TF>
            auto& else_(TF&&) noexcept
            {
                return *this;
            }
    };

    template <typename TF>
        auto make_static_if_result(TF&& f) noexcept
        {
            return static_if_result<TF>{FWD(f)};
        }
}

template <typename TPredicate>
auto static_if(TPredicate) noexcept
{
    return impl::static_if_impl<TPredicate{}>{};
}

//
// EXAMPLE
//
// struct banana   { void eat() { /* ... */ } };
// struct peanuts  { void eat() { /* ... */ } };
// struct water    { void drink() { /* ... */ } };
// struct juice    { void drink() { /* ... */ } };
//
// template <typename T> constexpr auto is_solid = bool_v<false>;
// template <>           constexpr auto is_solid<banana> = bool_v<true>;
// template <>           constexpr auto is_solid<peanuts> = bool_v<true>;
//
// template <typename T> constexpr auto is_liquid = bool_v<false>;
// template <>           constexpr auto is_liquid<water> = bool_v<true>;
// template <>           constexpr auto is_liquid<juice> = bool_v<true>;
// template <typename T>
// auto consume(T&& x)
// {
//     static_if(is_solid<T>)
//         .then([](auto&& y)
//               {
//                   y.eat();
//                   std::cout << "eating solid\n";
//               })
//         .else_if(is_liquid<T>)
//         .then([](auto&& y)
//               {
//                   y.drink();
//                   std::cout << "drinking liquid\n";
//               })
//         .else_([](auto&&)
//                {
//                    std::cout << "cannot consume\n";
//                })(FWD(x));
// }
//
// int main()
// {
//     consume(banana{});
//     consume(water{});
//     consume(peanuts{});
//     consume(juice{});
//     consume(int{});
//     consume(float{});
// }
//
