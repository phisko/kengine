#pragma once

#include <memory>
#include <tuple>
#include "meta/type.hpp"
#include "meta/GenLinearHierarchy.hpp"

namespace putils
{
    // Abstract Factory inherits from this for each type in TList
    template<typename T>
    class AbstractFactoryUnit
    {
        // Create a T
    public:
        virtual std::unique_ptr<T> makeImpl(pmeta::type<T>) noexcept = 0;
    };

    //
    // AbstractFactory able to build any type in TList
    // To implement a concrete factory, simply inherit from AbstractFactory
    // and overload std::unique_ptr<T>	make(pmeta::type<T>)
    //
    // To build an object of type T, use factory.make<T>()
    //
    template<typename ...TList>
    class AbstractFactory :
            public pmeta::GenLinearHierarchy<AbstractFactoryUnit, std::tuple<TList...>>
    {
        // Make an object of type T by casting myself to the right AbstractFactoryUnit
    public:
        template<typename T>
        std::unique_ptr<T> make() noexcept
        {
            AbstractFactoryUnit<T> &unit = static_cast<AbstractFactoryUnit<T> &>(*this);
            return unit.makeImpl(pmeta::type<T>());
        }
    };

    namespace test
    {
        inline void abstractFactory()
        {
            class CFactory : public AbstractFactory<int, double>
            {
                std::unique_ptr<int> makeImpl(pmeta::type<int>) noexcept final { return std::make_unique<int>(0); }
                std::unique_ptr<double> makeImpl(pmeta::type<double>) noexcept final { return std::make_unique<double>(42); }
            };

            CFactory factory;

            auto i = *(factory.make<int>());
            auto d = *(factory.make<double>());

            assert(i == 0);
            assert(d == 42);
        }
    }
}

