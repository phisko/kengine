#pragma once

#include <typeindex>

namespace pmeta
{
    using type_index = std::size_t;

    template<typename T>
    struct type
    {
        using wrapped = T;
        static const type_index index;
    };
}

template<typename T>
const pmeta::type_index pmeta::type<T>::index = std::hash<std::type_index>()(std::type_index(typeid(T)));