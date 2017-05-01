#include "GameObject.hpp"

namespace kengine
{
    void GameObject::detachComponent(const IComponent &comp)
    {
        const auto type = comp.getType();
        _components.erase(type);
        _types.erase(std::find(_types.begin(), _types.end(), type));
    }
}
