//
// Created by naliwe on 6/24/16.
//

#include <sstream>
#include "Component.hpp"

ComponentMask       Component::Mask    = ComponentMask::Default;
const bool          Component::_unique = false;


Component::Component(Component const& other)
{
    if (this != &other)
    {
        _name = other._name;
    }
}

Component& Component::operator=(Component other)
{
    swap(*this, other);

    return *this;
}

Component& Component::operator=(Component&& other)
{
    swap(*this, other);

    return *this;
}

bool Component::isUnique() const
{
    return _unique;
}

void swap(Component& left, Component& right)
{
    using std::swap;

    swap(left._name, right._name);
}

ComponentMask Component::getMask() const
{
    return Mask;
}

std::string Component::toString() const
{
    std::stringstream ss;

    ss << "Component:" << _name << std::endl;

    return ss.str();
}

