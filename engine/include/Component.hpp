//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_COMPONENT_HPP
# define KENGINE_COMPONENT_HPP

# include <string>
# include "IComponent.hpp"

namespace kengine
{
    template<ComponentMask TMask, bool TUnique = false>
    class Component : public IComponent
    {
    public:
        static constexpr ComponentMask Mask = TMask;
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
        virtual ComponentMask getMask() const noexcept override { return TMask; }
        std::string const& getName() const noexcept override { return _name; }
        virtual std::string toString() const noexcept override
        {
            return _name;
        }

    private:
        std::string       _name;
    };
}

#endif //KENGINE_COMPONENT_HPP
