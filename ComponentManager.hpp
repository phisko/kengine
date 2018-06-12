#pragma once

#include "Component.hpp"

namespace kengine {
    class ComponentManager {
    public:
        template<class CT, typename ...Params>
        CT & attachComponent(GameObject & parent, Params && ... params) const noexcept {
            return parent.attachComponent<CT>(FWD(params)...);
        };

    public:
        void detachComponent(GameObject & go, const IComponent & comp) const noexcept {
            go.detachComponent(comp);
        }

    public:
        template<typename T>
        const std::vector<GameObject *> & getGameObjects() noexcept {
            static_assert(kengine::is_component<T>::value,
                          "getGameObjects called without component type");
            return _entitiesByType[pmeta::type<T>::index].safe;
        }

        const std::vector<GameObject *> & getGameObjects() const noexcept { return _allEntities.safe; }

		void updateEntitiesByType() noexcept {
			_allEntities.safe = _allEntities.unsafe;
			for (auto & [type, category] : _entitiesByType)
				category.safe = category.unsafe;
        }

    protected:
        void registerGameObject(GameObject & go) noexcept {
			go.setManager(this);
            for (auto & [type, comp] : go._components)
                registerComponent(go, *comp);
            _allEntities.unsafe.push_back(&go);
        }

        void removeGameObject(const GameObject & go) noexcept {
            for (const auto type : go._types)
                removeComponent(go, type);
			const auto it = std::find(_allEntities.unsafe.begin(), _allEntities.unsafe.end(), &go);
			if (it != _allEntities.unsafe.end())
				_allEntities.unsafe.erase(it);
        }

    private:
        friend class GameObject;

        void registerComponent(GameObject & parent, const IComponent & comp) noexcept {
			_entitiesByType[comp.getType()].unsafe.push_back(&parent);
        }

		void removeComponent(const GameObject & go, pmeta::type_index type) noexcept {
			auto & category = _entitiesByType[type];
			const auto it = std::find(category.unsafe.begin(), category.unsafe.end(), &go);
			if (it != category.unsafe.end())
				category.unsafe.erase(it);
		}

    private:
		struct EntityCollection
		{
			std::vector<GameObject *> safe;
			std::vector<GameObject *> unsafe;
		};
        std::unordered_map<pmeta::type_index, EntityCollection> _entitiesByType;
        EntityCollection _allEntities;
    };
}