#include <sstream>
#include "GameObject.hpp"

namespace kengine
{
    GameObject::GameObject(std::string const &name)
            : _name(name) {}

    void GameObject::attachComponent(IComponent *comp)
    {
        this->addModule(comp);
        _components[comp->getName()] = comp;
        _types.push_back(comp->getType());
    }

    void GameObject::detachComponent(IComponent *comp)
    {
        _components.erase(comp->getName());
        _types.erase(std::find(_types.begin(), _types.end(), comp->getType()));
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

