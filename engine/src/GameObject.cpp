#include <sstream>
#include "GameObject.hpp"

namespace kengine
{
    GameObject::GameObject(std::string const &name)
            : _name(name) {}

    GameObject::GameObject(GameObject const &other)
    {
        if (this != &other)
        {
            _name = other._name;
            _components = other._components;
        }
    }

    GameObject &GameObject::operator=(GameObject other)
    {
        swap(*this, other);

        return *this;
    }

    GameObject &GameObject::operator=(GameObject &&other)
    {
        swap(*this, other);

        return *this;
    }

    GameObject::~GameObject() {}

    void swap(GameObject &left, GameObject &right)
    {
        using std::swap;

        swap(left._name, right._name);
        swap(left._components, right._components);
    }

    void GameObject::attachComponent(IComponent *comp)
    {
        _components[comp->get_name()] = comp;
        (unsigned char &) _mask |= (unsigned char) comp->getMask();
    }

    void GameObject::detachComponent(IComponent *comp)
    {
        _components.erase(comp->get_name());
    }

    std::string GameObject::toString() const
    {
        std::stringstream ss;

        ss << "{" << std::endl
           << "\tname: " << _name << ", " << std::endl
           << "\tcomponents: {" << std::endl;

        bool first = true;
        for (const auto &p : _components)
        {
            if (first)
                first = false;
            else
                ss << "," << std::endl;

            ss << "\t\t" << p.second->toString() << "";
        }
        ss << std::endl << "\t}" << std::endl
                << "}";

        return ss.str();
    }
}

