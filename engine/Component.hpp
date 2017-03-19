//
// Created by naliwe on 6/24/16.
//

#pragma once

#include <string>
#include "IComponent.hpp"

namespace kengine
{
    template<typename CRTP, bool TUnique = false, typename ...DataPackets>
    class Component : public IComponent, public putils::Module<DataPackets...>
    {
    public:
        static const pmeta::type_index Type;
        static constexpr bool Unique = TUnique;

    public:
        Component(std::string const& name)
                : _name(name)
        {}

        Component(const Component &other) = default;
        Component& operator=(const Component &other) = default;

        Component(Component &&other) = default;
        Component& operator=(Component&& other) = default;

        virtual ~Component() = default;

    public:
        bool isUnique() const noexcept override { return TUnique; }

    public:
        pmeta::type_index getType() const noexcept override { return Type; }
        std::string const& getName() const noexcept override { return _name; }
        std::string toString() const noexcept override
        {
            return _name;
        }

    private:
        std::string       _name;
    };

    template<typename CRTP, bool TUnique, typename ...DataPackets>
    const pmeta::type_index Component<CRTP, TUnique, DataPackets...>::Type = pmeta::type<CRTP>::index;
}
