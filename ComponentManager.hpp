#pragma once

// #include "GameObject.hpp"
#include "Component.hpp"

namespace kengine
{
    class ComponentManager
    {
    public:
        template<class CT, typename ...Params>
        CT &attachComponent(GameObject &parent, Params &&... params) noexcept
        {
/*
            auto ptr = std::make_unique<CT>(FWD(params)...);
            auto &comp = *ptr;

            _compHierarchy.emplace(&comp, &parent);
            parent.attachComponent(std::move(ptr));
            return comp;
*/

            return parent.attachComponent<CT>(FWD(params)...);
        };

    public:
        void detachComponent(GameObject &go, const IComponent &comp)
        {
            // _compHierarchy.erase(&comp);
            go.detachComponent(comp);
        }

    public:
        void registerGameObject(GameObject &go)
        {
            go.setManager(this);
            for (const auto & [type, comp] : go._components)
                registerComponent(go, *comp);
        }

    public:
        const GameObject &getParent(const IComponent &comp) const { return *_compHierarchy.at(&comp); }

    private:
        friend class GameObject;

        void registerComponent(const GameObject &parent, const IComponent &comp)
        {
            _compHierarchy.emplace(&comp, &parent);
        }

        void removeComponent(const IComponent &comp)
        {
            _compHierarchy.erase(&comp);
        }

    private:
        std::unordered_map<const IComponent *, const GameObject *> _compHierarchy;
    };
}