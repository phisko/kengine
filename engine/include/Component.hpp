//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_COMPONENT_HPP
# define KENGINE_COMPONENT_HPP

# include <string>
# include "IComponent.hpp"
# include "Object.hpp"

class Component : public IComponent
{
public:
    static ComponentMask Mask;
public:
    Component(std::string const& name, Object const *parent)
            : _parent(parent), _name(name)
    { }

    Component(Component const& other);
    Component(Component&& other);
    Component& operator=(Component other);
    Component& operator=(Component&& other);

    virtual ~Component()
    { }

public:
    bool isUnique() const;

public:
    friend void swap(Component& left, Component& right);

public:
    virtual ComponentMask getMask() const override;
    virtual std::string   toString() const override;

private:
    const Object      *_parent;
    std::string       _name;
    static const bool _unique;
};


#endif //KENGINE_COMPONENT_HPP
