//
// Created by naliwe on 6/24/16.
//

#pragma once

#include <string>
#include "IComponent.hpp"

namespace kengine
{
    template<typename CRTP, typename ...DataPackets>
    class Component : public IComponent, public putils::Module<CRTP, DataPackets...>
    {
    public:
        static const pmeta::type_index Type;

    public:
        Component()
        {}

        Component(const Component &other) = default;
        Component& operator=(const Component &other) = default;

        Component(Component &&other) = default;
        Component& operator=(Component&& other) = default;

        virtual ~Component() = default;

    public:
        pmeta::type_index getType() const noexcept override { return Type; }
    };

    template<typename CRTP, typename ...DataPackets>
    const pmeta::type_index Component<CRTP, DataPackets...>::Type = pmeta::type<CRTP>::index;
}
