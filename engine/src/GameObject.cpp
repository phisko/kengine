#include <sstream>
#include "GameObject.hpp"

GameObject::GameObject(std::string const& name)
        : _name(name)
{ }

GameObject::GameObject(GameObject const& other)
{
    if (this != &other)
    {
        _name = other._name;
        _components = _components;
    }
}


GameObject::GameObject(GameObject&& other)
{
    swap(*this, other);
}

GameObject& GameObject::operator=(GameObject other)
{
    swap(*this, other);

    return *this;
}

GameObject& GameObject::operator=(GameObject&& other)
{
    swap(*this, other);

    return *this;
}

GameObject::~GameObject()
{

}

void swap(GameObject& left, GameObject& right)
{
    using std::swap;

    swap(left._name, right._name);
    swap(left._components, right._components);
}

void GameObject::attachComponent(IComponent const& comp)
{
    (void)comp;
}

void GameObject::attachComponent(IComponent&& comp)
{
    (void)comp;
}

std::string GameObject::toString() const
{
    std::stringstream ss;

    ss << "GameObject:" << _name;

    return ss.str();
}
