#pragma once

#include "with.hpp"
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
			_allEntities.update();

			for (auto & [type, category] : _entitiesByType)
				category.update();
        }

    protected:
        void registerGameObject(GameObject & go) noexcept {
			go.setManager(this);
            for (auto & [type, comp] : go._components)
                registerComponent(go, *comp);
            _allEntities.unsafe.push_back(&go);
			_allEntities.changed = true;
        }

        void removeGameObject(const GameObject & go) noexcept {
            for (const auto type : go._types)
                removeComponent(go, type);
			const auto it = std::find(_allEntities.unsafe.begin(), _allEntities.unsafe.end(), &go);
			if (it != _allEntities.unsafe.end()) {
				_allEntities.unsafe.erase(it);
				_allEntities.changed = true;
			}
        }

    private:
        friend class GameObject;

        void registerComponent(GameObject & parent, const IComponent & comp) noexcept {
			auto & category = _entitiesByType[comp.getType()];
			category.unsafe.push_back(&parent);
			category.changed = true;
        }

		void removeComponent(const GameObject & go, pmeta::type_index type) noexcept {
			auto & category = _entitiesByType[type];
			const auto it = std::find(category.unsafe.begin(), category.unsafe.end(), &go);
			if (it != category.unsafe.end()) {
				category.unsafe.erase(it);
				category.changed = true;
			}
		}

    private:
		struct EntityCollection
		{
			bool changed = false;
			std::vector<GameObject *> safe;
			std::vector<GameObject *> unsafe;

			void update() {
				if (!changed)
					return;
				safe = unsafe;
				changed = false;
			}
		};
        std::unordered_map<pmeta::type_index, EntityCollection> _entitiesByType;
        EntityCollection _allEntities;
    };
}