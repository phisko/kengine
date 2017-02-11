//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_COMPONENT_HPP
# define KENGINE_COMPONENT_HPP

# include <string>
# include "GameObject.hpp"

class Component
{
public:
    Component(std::string const& name, GameObject const *const parent)
            : _parent(parent), _name(name)
    { }

    Component(Component const& other);
    Component(Component&& other);
    Component& operator=(Component other);

    virtual ~Component()
    { }

public:
    friend void swap(Component& left, Component& right);

protected:
    GameObject const *const _parent;
    std::string       _name;
    static const bool _unique;
};


#endif //KENGINE_COMPONENT_HPP
