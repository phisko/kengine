#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ThreadPool.hpp"
#include "SystemManager.hpp"
#include "Component.hpp"
#include "Entity.hpp"

namespace kengine {
    class EntityManager : public SystemManager {
    public:
		EntityManager(size_t threads = 0) : SystemManager(threads) {}

    public:
		template<typename Func> // Func: void(Entity &);
        Entity & createEntity(Func && postCreate) {
			auto & e = alloc();
			postCreate(e);
			SystemManager::registerEntity(e);
			return e;
        }

		template<typename Func>
		Entity & operator+=(Func && postCreate) {
			return createEntity(FWD(postCreate));
		}

    private:
		auto iteratorFor(size_t id) const {
			const auto it = std::find_if(_entities.begin(), _entities.end(), [id](const auto & e) { return e.id == id; });
			assert("No such entity" && it != _entities.end());
			return it;
		}

		auto iteratorFor(size_t id) {
			const auto it = std::find_if(_entities.begin(), _entities.end(), [id](const auto & e) { return e.id == id; });
			assert("No such entity" && it != _entities.end());
			return it;
		}

	public:
		Entity & getEntity(size_t id) {
			return *iteratorFor(id);
		}

		const Entity & getEntity(size_t id) const {
			 return *iteratorFor(id);
		}

    public:
		void removeEntity(const EntityView & e) {
			removeEntity(e.id);
		}

		void removeEntity(size_t id) {
			const auto it = iteratorFor(id);
			SystemManager::removeEntity(*it);

			for (const auto & [type, meta] : _components)
				meta->swap(id, _count - 1);

			auto & lastMask = _entities[_count - 1].componentMask;
			for (auto & collection : _entitiesByType) {
				if (collection.mask == it->componentMask) {
					const auto size = collection.entities.size();
					if (size > 1 && lastMask != it->componentMask) {
						const auto tmp = std::find_if(collection.entities.begin(), collection.entities.end(),
							[id](EntityView v) { return v.id == id; });
						std::iter_swap(tmp, collection.entities.begin() + size - 1);
					}
					collection.entities.pop_back();
					break;
				}
			}

			if (_count > 1)
				it->componentMask = lastMask;
			lastMask = 0;

			--_count;
		}

	private:
		struct EntityType {
			Entity::Mask mask;
			std::vector<EntityView> entities;
		};

		struct EntityCollection {
			struct EntityIterator {
				EntityIterator & operator++() {
					++index;
					return *this;
				}

				bool operator!=(const EntityIterator & rhs) const {
					return index != rhs.index;
				}

				Entity & operator*() const {
					return vec[index];
				}

				std::vector<Entity> & vec;
				size_t maxSize;
				size_t index = 0;
			};

			auto begin() const {
				return EntityIterator{ vec, count, 0 };
			}

			auto end() const {
				return EntityIterator{ vec, count, count };
			}

			std::vector<Entity> & vec;
			size_t count;
		};

	public:
		auto getEntities() {
			return EntityCollection{ _entities, _count };
		}

	private:
		template<typename ... Comps>
		struct ComponentCollection {
			struct ComponentIterator {
				ComponentIterator & operator++() {
					++currentEntity;
					if (currentEntity < entitiesByType[currentType].entities.size())
						return *this;

					currentEntity = 0;
					for (++currentType; currentType < entitiesByType.size(); ++currentType) {
						if (entitiesByType[currentType].entities.empty())
							continue;

						bool good = true;
						pmeta_for_each(Comps, [&](auto && type) {
							using CompType = pmeta_wrapped(type);
							if (!entitiesByType[currentType].mask[Component<CompType>::id()])
								good = false;
						});
						if (good)
							break;
					}

					return *this;
				}

				bool operator!=(const ComponentIterator & rhs) const { return currentType != rhs.currentType; }

				std::tuple<EntityView &, Comps &...> operator*() const {
					auto & e = entitiesByType[currentType].entities[currentEntity];
					return std::make_tuple(std::ref(e), std::ref(e.get<Comps>())...);
				}

				std::vector<EntityType> & entitiesByType;
				size_t currentType;
				size_t currentEntity;
			};

			auto begin() const {
				size_t i = 0;
				for (; i < entitiesByType.size(); ++i) {
					if (entitiesByType[i].entities.empty())
						continue;

					bool good = true;
					pmeta_for_each(Comps, [&](auto && type) {
						using CompType = pmeta_wrapped(type);
						if (!entitiesByType[i].mask[Component<CompType>::id()])
							good = false;
					});
					if (good)
						break;
				}
				
				return ComponentIterator{ entitiesByType, i, 0 };
			}

			auto end() const {
				return ComponentIterator{ entitiesByType, entitiesByType.size(), 0 };
			}

			std::vector<EntityType> & entitiesByType;
		};

    public:
		template<typename ... Comps>
		auto getEntities() {
			return ComponentCollection<Comps...>{ _entitiesByType };
		}

    public:
        template<typename RegisterWith, typename ...Types>
        void registerTypes() {
            if constexpr (!std::is_same<RegisterWith, nullptr_t>::value) {
				auto & s = getSystem<RegisterWith>();
				s.template registerTypes<Types...>();
            }
		}

	private:
		Entity & alloc() {
			if (_count == _entities.size())
				_entities.emplace_back(Entity{ _entities.size(), this });
			return _entities[_count++];
		}

    private:
		friend class Entity;
		void updateMask(Entity e, Entity::Mask oldMask) {
			int done = 0;
			if (oldMask == 0ll) // Will not have to remove
				++done;

			for (auto & collection : _entitiesByType) {
				if (collection.mask == oldMask) {
					const auto size = collection.entities.size();
					if (size > 1) {
						const auto it = std::find_if(collection.entities.begin(), collection.entities.end(),
							[id = e.id](EntityView other) { return other.id == id; });
						std::iter_swap(it, collection.entities.begin() + size - 1);
					}
					collection.entities.pop_back();
					++done;
				}

				if (collection.mask == e.componentMask) {
					collection.entities.emplace_back(e);
					++done;
				}

				if (done == 2)
					break;
			}

			if (done == 1)
				_entitiesByType.emplace_back(EntityType{ e.componentMask, { e }});
		}

	private:
		std::vector<Entity> _entities;
		std::vector<EntityType> _entitiesByType;
		size_t _count = 0;

	private:
		detail::GlobalCompMap _components;

	public: // Reserved to systems
		detail::GlobalCompMap & __getComponentMap() { return _components; }
	};
}
