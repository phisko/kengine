#pragma once

// #include "GameObject.hpp"
#include "Component.hpp"

namespace kengine
{
    class ComponentManager
    {
    public:
        template<class CT, typename ...Params>
        CT &attachComponent(GameObject &parent, Params &&... params) const noexcept
        {
            return parent.attachComponent<CT>(FWD(params)...);
        };

    public:
        void detachComponent(GameObject &go, const IComponent &comp) const
        {
            go.detachComponent(comp);
        }

    public:
        const GameObject &getParent(const IComponent &comp) const { return *_compHierarchy.at(&comp); }

    public:
        template<typename T>
        const std::vector<GameObject *> &getGameObjects()
        {
            static_assert(kengine::is_component<T>::value, "getGameObjects called without component type");
            return _entitiesByType[pmeta::type<T>::index];
        }

        const std::vector<GameObject *> &getGameObjects() const { return _allEntities; }

    protected:
        void registerGameObject(GameObject &go)
        {
            go.setManager(this);
            for (auto & [type, comp] : go._components)
                registerComponent(go, *comp);
        }

        void removeGameObject(const GameObject &go)
        {
            for (auto & [type, comp] : go._components)
                removeComponent(go, *comp);
        }

    private:
        friend class GameObject;

        void registerComponent(GameObject &parent, const IComponent &comp)
        {
            _compHierarchy.emplace(&comp, &parent);
            _entitiesByType[comp.getType()].push_back(&parent);
            _allEntities.push_back(&parent);
        }

        void removeComponent(const GameObject &go, const IComponent &comp)
        {
            _compHierarchy.erase(&comp);

            auto &category = _entitiesByType[comp.getType()];
            category.erase(std::find(category.begin(), category.end(), &go));

            _allEntities.erase(std::find(_allEntities.begin(), _allEntities.end(), &go));
        }

    private:
        std::unordered_map<const IComponent *, const GameObject *> _compHierarchy;
        std::unordered_map<pmeta::type_index, std::vector<GameObject *>> _entitiesByType;
        std::vector<GameObject *> _allEntities;
    };
}