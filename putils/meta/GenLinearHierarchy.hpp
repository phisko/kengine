#pragma once

#include <tuple>

namespace pmeta
{
    //
    // Class which, for each type X in Tuple,
    // inherits from T<X>
    //
    template<
            template<class> class T,
            typename Tuple>
    class GenLinearHierarchy;

    // Empty tuple specialization (called when Tuple is empty)
    template<template<class> class T>
    class GenLinearHierarchy<T, std::tuple<>> {};

    //
    // Typical specialization: inherits from T<U>
    // (with U being the first item in Tuple),
    // pops U from Tuple and calls itself again
    //
    template<
            template<class> class T,
            typename U,
            typename ...TList>
    class GenLinearHierarchy<T, std::tuple<U, TList...>>
            :
                    public T<U>,
                    public GenLinearHierarchy<T, std::tuple<TList...>>
    {
    };
}
