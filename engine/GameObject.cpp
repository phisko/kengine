#include <sstream>
#include "GameObject.hpp"

#include "concat.hpp"
#include "prettyprint.hpp"

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

    std::string GameObject::toString() const
    {
        std::string ret;

        ret = putils::concat("{name:", _name, ", components:[");

        bool first = true;
        for (const auto &p : _components)
        {
            if (first)
                first = false;
            else
                ret.append(1, ',');

            ret += p.second->toString();
        }
        ret += "]}";

        return putils::prettyPrint::json(std::move(ret));
    }
}

