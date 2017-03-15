//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_COMPONENT_HPP
# define KENGINE_COMPONENT_HPP

# include <string>
# include "IComponent.hpp"

namespace kengine
{
    template<ComponentMask Mask, bool Unique = false>
    class Component : public IComponent
    {
    public:
        Component(std::string const& name)
                : _name(name)
        {}

        Component(Component const& other) = default;
        Component& operator=(Component other) { swap(*this, other); return *this; }
        Component& operator=(Component&& other) { swap(*this, other); return *this; }

        virtual ~Component()
        {}

    public:
        bool isUnique() const { return Unique; }

    public:
        friend void swap(Component& left, Component& right)
        {
            using std::swap;
            swap(left._name, right._name);
        }

    public:
        virtual ComponentMask getMask() const noexcept override { return Mask; }
        std::string const& get_name() const noexcept override { return _name; }
        virtual std::string toString() const noexcept override
        {
            using namespace std::literals;
            return "Component"s + _name;
        }

    private:
        std::string       _name;
    };
}

#endif //KENGINE_COMPONENT_HPP
