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
        void detachComponent(GameObject &go, const IComponent &comp) const noexcept
        {
            go.detachComponent(comp);
        }

    public:
        const GameObject &getParent(const IComponent &comp) const { return *_compHierarchy.at(&comp); }

    public:
        template<typename T>
        const std::vector<GameObject *> &getGameObjects() noexcept
        {
            static_assert(kengine::is_component<T>::value,
                          "getGameObjects called without component type");
            return _entitiesByType[pmeta::type<T>::index];
        }

        const std::vector<GameObject *> &getGameObjects() const noexcept { return _allEntities; }

    protected:
        void registerGameObject(GameObject &go) noexcept
        {
            go.setManager(this);
            for (auto & [type, comp] : go._components)
                registerComponent(go, *comp);
            _allEntities.push_back(&go);
        }

        void removeGameObject(const GameObject &go) noexcept
        {
            for (auto & [type, comp] : go._components)
                removeComponent(go, *comp);

            _allEntities.erase(std::find(_allEntities.begin(), _allEntities.end(), &go));
        }

    private:
        friend class GameObject;

        void registerComponent(GameObject &parent, const IComponent &comp) noexcept
        {
            _compHierarchy.emplace(&comp, &parent);
            _entitiesByType[comp.getType()].push_back(&parent);
        }

        void removeComponent(const GameObject &go, const IComponent &comp) noexcept
        {
            _compHierarchy.erase(&comp);

            auto &category = _entitiesByType[comp.getType()];
            category.erase(std::find(category.begin(), category.end(), &go));
        }

    private:
        std::unordered_map<const IComponent *, const GameObject *> _compHierarchy;
        std::unordered_map<pmeta::type_index, std::vector<GameObject *>> _entitiesByType;
        std::vector<GameObject *> _allEntities;
    };
}