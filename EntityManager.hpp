#pragma once

#include <execution>
#include <unordered_map>
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
		auto iteratorFor(Entity::ID id) const {
			const auto it = std::find_if(std::execution::par_unseq, _entities.begin(), _entities.end(), [id](const auto & e) { return e.id == id; });
			assert("No such entity" && it != _entities.end());
			return it;
		}

		auto iteratorFor(Entity::ID id) {
			const auto it = std::find_if(std::execution::par_unseq, _entities.begin(), _entities.end(), [id](const auto & e) { return e.id == id; });
			assert("No such entity" && it != _entities.end());
			return it;
		}

	public:
		Entity & getEntity(Entity::ID id) {
			return *iteratorFor(id);
		}

		const Entity & getEntity(Entity::ID id) const {
			 return *iteratorFor(id);
		}

    public:
		void removeEntity(const EntityView & e) {
			removeEntity(e.id);
		}

		void removeEntity(Entity::ID id) {
			const auto it = iteratorFor(id);
			SystemManager::removeEntity(*it);

			for (auto & collection : _archetypes) {
				if (collection.mask == it->componentMask) {
					const auto tmp = std::find(std::execution::par_unseq, collection.entities.begin(), collection.entities.end(), id);
					if (collection.entities.size() > 1) {
						std::swap(*tmp, collection.entities.back());
						collection.sorted = false;
					}
					collection.entities.pop_back();
					break;
				}
			}

			it->componentMask = 0;
			std::swap(*it, _entities.back());
			--_count;
		}

	private:
		struct Archetype {
			Entity::Mask mask;
			std::vector<Entity::ID> entities;
			bool sorted = true;

			template<typename ... Comps>
			bool matches() {
				if (entities.empty())
					return false;

				if (!sorted) {
					std::sort(std::execution::par_unseq, entities.begin(), entities.end());
					sorted = true;
				}

				bool good = true;
				pmeta_for_each(Comps, [&](auto && type) {
					using CompType = pmeta_wrapped(type);
					if (!mask[Component<CompType>::id()])
						good = false;
				});
				return good;
			}
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
					return entities[index];
				}

				std::vector<Entity> & entities;
				size_t maxSize;
				size_t index = 0;
			};

			auto begin() const {
				return EntityIterator{ entities, count, 0 };
			}

			auto end() const {
				return EntityIterator{ entities, count, count };
			}

			std::vector<Entity> & entities;
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
				using iterator_category = std::forward_iterator_tag;
				using value_type = std::tuple<EntityView, Comps & ...>;
				using reference = const value_type &;
				using pointer = const value_type *;
				using difference_type = size_t;

				ComponentIterator & operator++() {
					++currentEntity;
					if (currentEntity < (*archetypes)[currentType].entities.size())
						return *this;

					currentEntity = 0;
					for (++currentType; currentType < archetypes->size(); ++currentType)
						if ((*archetypes)[currentType].matches<Comps...>())
							break;

					return *this;
				}

				bool operator==(const ComponentIterator & rhs) const { return currentType == rhs.currentType && currentEntity == rhs.currentEntity; }
				bool operator!=(const ComponentIterator & rhs) const { return !(*this == rhs); }

				std::tuple<EntityView, Comps &...> operator*() const {
					auto & archetype = (*archetypes)[currentType];
					EntityView e(archetype.entities[currentEntity], archetype.mask);
					return std::make_tuple(e, std::ref(e.get<Comps>())...);
				}

				std::vector<Archetype> * archetypes;
				size_t currentType;
				size_t currentEntity;
			};

			auto begin() const {
				size_t i = 0;
				for (; i < archetypes.size(); ++i)
					if (archetypes[i].matches<Comps...>())
						break;
				
				return ComponentIterator{ &archetypes, i, 0 };
			}

			auto end() const {
				return ComponentIterator{ &archetypes, archetypes.size(), 0 };
			}

			std::vector<Archetype> & archetypes;
		};

    public:
		template<typename ... Comps>
		auto getEntities() {
			return ComponentCollection<Comps...>{ _archetypes };
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

			for (auto & collection : _archetypes) {
				if (collection.mask == oldMask) {
					const auto size = collection.entities.size();
					if (size > 1) {
						const auto it = std::find(std::execution::par_unseq, collection.entities.begin(), collection.entities.end(), e.id);
						std::iter_swap(it, collection.entities.begin() + size - 1);
					}
					collection.entities.pop_back();
					collection.sorted = false;
					++done;
				}

				if (collection.mask == e.componentMask) {
					collection.entities.emplace_back(e.id);
					collection.sorted = false;
					++done;
				}

				if (done == 2)
					break;
			}

			if (done == 1)
				_archetypes.emplace_back(Archetype{ e.componentMask, { e.id }});
		}

	private:
		std::vector<Entity> _entities;
		std::vector<Archetype> _archetypes;
		size_t _count = 0;

	private:
		detail::GlobalCompMap _components;

	public: // Reserved to systems
		detail::GlobalCompMap & __getComponentMap() { return _components; }
	};
}
