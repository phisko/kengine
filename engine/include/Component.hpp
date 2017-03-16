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

        Component(Component const& other) = default;
        Component& operator=(Component other) { swap(*this, other); return *this; }
        Component& operator=(Component&& other) { swap(*this, other); return *this; }

        virtual ~Component()
        {}

    public:
        bool isUnique() const { return TUnique; }

    public:
        friend void swap(Component& left, Component& right)
        {
            using std::swap;
            swap(left._name, right._name);
        }

    public:
        virtual ComponentMask getMask() const noexcept override { return TMask; }
        std::string const& get_name() const noexcept override { return _name; }
        virtual std::string toString() const noexcept override
        {
            return _name;
        }

    private:
        std::string       _name;
    };
}

#endif //KENGINE_COMPONENT_HPP
