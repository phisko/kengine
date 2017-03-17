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
        _mask.push_back(comp->getMask());
    }

    void GameObject::detachComponent(IComponent *comp)
    {
        _components.erase(comp->getName());
        _mask.erase(std::find(_mask.begin(), _mask.end(), comp->getMask()));
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

