#pragma once

#include "GameObject.hpp"
#include "Component.hpp"

namespace kengine
{
    class ComponentManager
    {
    public:
        template<class CT, typename ...Params>
        CT &attachComponent(GameObject &parent, Params &&... params) noexcept
        {
            auto ptr = std::make_unique<CT>(FWD(params)...);
            auto &comp = *ptr;

            _compHierarchy.emplace(&comp, &parent);
            parent.attachComponent(std::move(ptr));

            return comp;
        };

    public:
        void detachComponent(GameObject &go, const IComponent &comp)
        {
            _compHierarchy.erase(&comp);
            go.detachComponent(comp);
        }

    public:
        void registerGameObject(const GameObject &go)
        {
            for (const auto & [type, comp] : go._components)
                _compHierarchy.emplace(comp.get(), &go);
        }

    public:
        const GameObject &getParent(const IComponent &comp) const { return *_compHierarchy.at(&comp); }

    private:
        std::unordered_map<const IComponent *, const GameObject *> _compHierarchy;
    };
}