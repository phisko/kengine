#include <sstream>
#include "GameObject.hpp"

namespace kengine
{
    GameObject::GameObject(std::string const &name)
            : _name(name) {}

    void GameObject::attachComponent(IComponent *comp)
    {
        this->addModule(comp);
        _componentsByName[comp->getName()] = comp;
        const auto type = comp->getType();
        _componentsByType[type] = comp;
        _types.push_back(type);
    }

    void GameObject::detachComponent(IComponent *comp)
    {
        _componentsByName.erase(comp->getName());
        const auto type = comp->getType();
        _componentsByType.erase(type);
        _types.erase(std::find(_types.begin(), _types.end(), type));
    }

    std::string GameObject::toString() const
    {
        std::stringstream ss;

        ss << "{" << std::endl
           << "\tname: " << _name << ", " << std::endl
           << "\tcomponents: {" << std::endl;

        bool first = true;
        for (const auto &p : _componentsByName)
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

