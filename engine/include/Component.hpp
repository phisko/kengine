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
        static const std::size_t Mask;
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
        void receive(const putils::ADataPacket &) noexcept override {}

    public:
        bool isUnique() const noexcept override { return TUnique; }

    public:
        virtual std::size_t getMask() const noexcept override { return Mask; }
        std::string const& getName() const noexcept override { return _name; }
        virtual std::string toString() const noexcept override
        {
            return _name;
        }

    private:
        std::string       _name;
    };

    template<typename CRTP, bool TUnique, typename ...DataPackets>
    const std::size_t Component<CRTP, TUnique, DataPackets...>::Mask = pmeta::type<CRTP>::index;
}
