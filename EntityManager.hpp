#pragma once

#include <vector>
#include "SystemManager.hpp"
#include "Component.hpp"
#include "Entity.hpp"

namespace kengine {
    class EntityManager : public SystemManager {
    public:
        Entity & createEntity(const std::function<void(Entity &)> & postCreate = nullptr) {
			auto & e = alloc();
            if (postCreate != nullptr)
                postCreate(e);
			return e;
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
		void removeEntity(const Entity & e) {
			removeEntity(e.id);
		}

		void removeEntity(size_t id) { assert("Invalid id" && id < _count);
			const auto it = iteratorFor(id);
			std::iter_swap(it, _entities.begin() + _count);
			--_count;
		}

	private:
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
					++index;

					while (index < maxSize) {
						bool good = true;
						pmeta_for_each(Comps, [&](auto && type) {
							using CompType = pmeta_wrapped(type);
							if (!vec[index].has<CompType>())
								good = false;
						});
						if (good)
							break;
						++index;
					}

					return *this;
				}

				bool operator!=(const ComponentIterator & rhs) const { return index != rhs.index; }

				std::tuple<Entity &, Comps &...> operator*() const {
					auto & e = vec[index];
					return std::make_tuple(std::ref(e), std::ref(e.get<Comps>())...);
				}

				std::vector<Entity> & vec;
				size_t maxSize;
				size_t index;
			};

			auto begin() const {
				size_t i = 0;
				while (i < count) {
					bool good = true;
					pmeta_for_each(Comps, [&](auto && type) {
						using CompType = pmeta_wrapped(type);
						if (!vec[i].has<CompType>())
							good = false;
					});
					if (good)
						break;
					++i;
				}
				
				return ComponentIterator{ vec, count, i };
			}

			auto end() const {
				return ComponentIterator{ vec, count, count };
			}

			std::vector<Entity> & vec;
			size_t count;
		};

    public:
		template<typename ... Comps>
		auto getEntities() {
			return ComponentCollection<Comps...>{ _entities, _count };
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
				_entities.emplace_back(Entity{ ++_nextId });
			return _entities[_count++];
		}

	private:
		std::vector<Entity> _entities;
		size_t _count = 0;
		size_t _nextId = 0;

	private:
		detail::GlobalCompMap _components;

	public: // Reserved to systems
		detail::GlobalCompMap & __getComponentMap() { return _components; }
	};
}
