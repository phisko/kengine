#include <sstream>
#include "GameObject.hpp"


namespace kengine
{
    GameObject::GameObject(std::string const &name)
            : _name(name) {}

    void GameObject::attachComponent(IComponent *comp)
    {
        this->addModule(comp);
        const auto type = comp->getType();
        _components[type] = comp;
        _types.push_back(type);
    }

    void GameObject::detachComponent(const IComponent *comp)
    {
        const auto type = comp->getType();
        _components.erase(type);
        _types.erase(std::find(_types.begin(), _types.end(), type));
    }
}

