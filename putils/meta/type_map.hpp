#pragma once

#include <tuple>
#include "fwd.hpp"
#include "type.hpp"

namespace pmeta
{
    auto type_map(auto &&...keyValues);

    // func: Function which will take the pmeta::type as parameter
    void get_type(auto &&key, auto &&type_map, auto &&func);

    template<typename T>
    auto get_key(auto &&type_map);

    namespace detail
    {
        template<std::size_t ...Is>
        void get_type(auto &&key, auto &&type_map, auto &&func, std::index_sequence<Is...>);

        void get_type(auto &&key, auto &&type_map, auto &&func, std::index_sequence<>)
        { throw std::out_of_range("No such type"); }

        enum Indexes
        {
            TypeName = 0,
            Type = 1
        };

        template<std::size_t I, std::size_t ...Is>
        void get_type(auto &&key, auto &&type_map, auto &&func, std::index_sequence<I, Is...>)
        {
            const auto &pair = std::get<I>(type_map);

            if (std::get<TypeName>(pair) == key)
            {
                func(std::get<Type>(pair));
                return;
            }

            get_type(key, type_map, func, std::index_sequence<Is...>());
        }

        template<typename ...Pairs>
        void get_type(auto &&key, const std::tuple<Pairs...> &type_map, auto &&func)
        {
            get_type(key, type_map, func, std::index_sequence_for<Pairs...>());
        }

        template<std::size_t ...Is>
        auto get_key(auto &&type_map, std::index_sequence<Is...>);

        template<typename T, std::size_t I>
        auto get_key(auto &&type_map, std::index_sequence<I>)
        {
            const auto &pair = std::get<I>(type_map);

            using WrapperType = std::tuple_element_t<Type, std::decay_t<decltype(pair)>>;
            using ObjectType = typename WrapperType::wrapped;

            if (std::is_same<ObjectType, T>::value)
                return std::get<TypeName>(pair);

            throw std::out_of_range("No such type");
        }

        template<typename T, std::size_t I, std::size_t ...Is>
        auto get_key(auto &&type_map, std::index_sequence<I, Is...>)
        {
            const auto &pair = std::get<I>(type_map);

            using WrapperType = std::tuple_element_t<Type, std::decay_t<decltype(pair)>>;
            using ObjectType = typename WrapperType::wrapped;

            if (std::is_same<ObjectType, T>::value)
                return std::get<TypeName>(pair);

            return get_key<T>(type_map, std::index_sequence<Is...>());
        };

        template<typename T, typename ...Pairs>
        auto get_key(const std::tuple<Pairs...> &type_map)
        {
            return get_key<T>(type_map, std::index_sequence_for<Pairs...>());
        }
    }

    void get_type(auto &&key, auto &&type_map, auto &&func) { detail::get_type(key, type_map, func); }

    template<typename T>
    auto get_key(auto &&type_map) { return detail::get_key<T>(type_map); }

    inline auto type_map() { return std::make_tuple(); }

    auto type_map(auto &&key, auto &&type, auto &&...args)
    {
        return std::tuple_cat(
                std::make_tuple(
                        std::make_tuple(FWD(key), FWD(type))
                ),
                type_map(FWD(args)...)
        );
    }
}