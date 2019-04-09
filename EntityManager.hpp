#pragma once

#include <unordered_map>
#include <vector>
#include "SystemManager.hpp"
#include "Component.hpp"
#include "Entity.hpp"

namespace kengine {
    class EntityManager : public SystemManager {
    public:
		EntityManager(size_t threads = 0) : SystemManager(threads) {}

    public:
		template<typename Func> // Func: void(Entity &);
        Entity createEntity(Func && postCreate) {
			auto e = alloc();
			postCreate(e);
			SystemManager::registerEntity(e);
			return e;
        }

		template<typename Func>
		Entity operator+=(Func && postCreate) {
			return createEntity(FWD(postCreate));
		}

	public:
		Entity getEntity(Entity::ID id) {
			return Entity(id, _entities[id], this);
		}

		EntityView getEntity(Entity::ID id) const {
			return EntityView(id, _entities[id]);
		}

    public:
		void removeEntity(EntityView e) {
			removeEntity(e.id);
		}

		void removeEntity(Entity::ID id) {
			if (_updatesLocked)
				_removals.push_back({ id });
			else 
				doRemove(id);
		}

    public:
		void load() {
			_archetypes.clear();
			_toReuse.clear();

			for (size_t i = 0; i < _entities.size(); ++i)
				SystemManager::removeEntity(EntityView(i, _entities[i]));

			for (const auto &[_, meta] : _components)
				meta->load();

			std::ifstream f("entities.bin");
			size_t size;
			f.read((char *)&size, sizeof(size));
			_entities.resize(size);
			f.read((char *)_entities.data(), size * sizeof(Entity::Mask));

			for (size_t i = 0; i < _entities.size(); ++i) {
				const auto mask = _entities[i];
				if (mask != 0) {
					updateMask(i, mask, true);
					Entity e{ i, mask, this };
					SystemManager::registerEntity(e);
				} else
					_toReuse.emplace_back(i);
			}

			SystemManager::load();
		}

		void save() {
			SystemManager::save();

			std::vector<bool> serializable;

			serializable.resize(_components.size());
			for (const auto &[_, meta] : _components)
				serializable[meta->getId()] = meta->save();

			std::ofstream f("entities.bin");
			const auto size = _entities.size();
			f.write((const char *)&size, sizeof(size));
			for (Entity::Mask mask : _entities) {
				for (size_t i = 0; i < serializable.size(); ++i)
					mask[i] = mask[i] & serializable[i];
				f.write((const char *)&mask, sizeof(mask));
			}
		}


	private:
		struct Archetype {
			Entity::Mask mask;
			std::vector<Entity::ID> entities;
			bool sorted = true;

			Archetype(Entity::Mask mask, Entity::ID firstEntity) : mask(mask), entities({ firstEntity }) {}

			template<typename ... Comps>
			bool matches() {
				if (entities.empty())
					return false;

				if (!sorted) {
					std::sort(entities.begin(), entities.end());
					sorted = true;
				}

				bool good = true;
				pmeta_for_each(Comps, [&](auto && type) {
					using CompType = pmeta_wrapped(type);
					good &= mask.test(Component<CompType>::id());
				});
				return good;
			}
		};

		struct EntityCollection {
			struct EntityIterator {
				EntityIterator & operator++() {
					++index;
					while (index < em._entities.size() && em._entities[index] == 0)
						++index;
					return *this;
				}

				bool operator!=(const EntityIterator & rhs) const {
					return index != rhs.index;
				}

				Entity operator*() const {
					return Entity(index, em._entities[index], &em);
				}

				size_t index;
				EntityManager & em;
			};

			auto begin() const {
				size_t i = 0;
				while (i < em._entities.size() && em._entities[i] == 0)
					++i;
				return EntityIterator{ i, em };
			}

			auto end() const {
				return EntityIterator{ em._entities.size(), em };
			}

			EntityCollection(EntityManager & em) : em(em) {
				++em._updatesLocked;
			}

			~EntityCollection() {
				assert(em._updatesLocked != 0);
				--em._updatesLocked;
				if (em._updatesLocked == 0)
					em.doAllUpdates();
			}

			EntityManager & em;
		};

	public:
		auto getEntities() {
			return EntityCollection{ *this };
		}

	private:
		template<typename ... Comps>
		struct ComponentCollection {
			struct ComponentIterator {
				using iterator_category = std::forward_iterator_tag;
				using value_type = std::tuple<Entity, Comps & ...>;
				using reference = const value_type &;
				using pointer = const value_type *;
				using difference_type = size_t;

				ComponentIterator & operator++() {
					++currentEntity;
					if (currentEntity < em._archetypes[currentType].entities.size())
						return *this;

					currentEntity = 0;
					for (++currentType; currentType < em._archetypes.size(); ++currentType)
						if (em._archetypes[currentType].matches<Comps...>())
							break;

					return *this;
				}

				bool operator==(const ComponentIterator & rhs) const { return currentType == rhs.currentType && currentEntity == rhs.currentEntity; }
				bool operator!=(const ComponentIterator & rhs) const { return !(*this == rhs); }

				std::tuple<Entity, Comps &...> operator*() const {
					const auto & archetype = em._archetypes[currentType];
					Entity e(archetype.entities[currentEntity], archetype.mask, &em);
					return std::make_tuple(e, std::ref(e.get<Comps>())...);
				}

				kengine::EntityManager & em;
				size_t currentType;
				size_t currentEntity;
			};

			auto begin() const {
				size_t i = 0;
				for (; i < em._archetypes.size(); ++i)
					if (em._archetypes[i].matches<Comps...>())
						break;
				
				return ComponentIterator{ em, i, 0 };
			}

			auto end() const {
				return ComponentIterator{ em, em._archetypes.size(), 0 };
			}

			ComponentCollection(EntityManager & em) : em(em) {
				++em._updatesLocked;
			}

			~ComponentCollection() {
				assert(em._updatesLocked != 0);
				--em._updatesLocked;
				if (em._updatesLocked == 0)
					em.doAllUpdates();
			}

			EntityManager & em;
		};

    public:
		template<typename ... Comps>
		auto getEntities() {
			return ComponentCollection<Comps...>{ *this };
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
		Entity alloc() {
			if (_toReuse.empty()) {
				const auto id = _entities.size();
				_entities.emplace_back(0);
				return Entity(id, 0, this);
			}

			if (!_toReuseSorted) {
				std::sort(_toReuse.begin(), _toReuse.end());
				_toReuseSorted = true;
			}

			const auto id = _toReuse.back();
			_toReuse.pop_back();

#ifndef NDEBUG
			for (const auto & archetype : _archetypes)
				assert(std::find(archetype.entities.begin(), archetype.entities.end(), id) == archetype.entities.end());
#endif

			assert(id < _entities.size());
			return Entity(id, 0, this);
		}

    private:
		friend class Entity;
		void addComponent(Entity::ID id, size_t component, Entity::Mask updatedMaskForCheck) {
			auto currentMask = _entities[id];
			for (auto & update : _updates)
				if (update.id == id) {
					update.newMask[component] = true;
					// Commented out for now as objects in ComponentCollections may have obsolete masks
					// assert(update.newMask == updatedMaskForCheck);
					return;
				}

			auto updatedMask = currentMask;
			updatedMask[component] = true;
			assert(updatedMask == updatedMaskForCheck);

			updateMask(id, updatedMask);
		}
		
		void removeComponent(Entity::ID id, size_t component, Entity::Mask updatedMaskForCheck) {
			auto currentMask = _entities[id];
			for (auto & update : _updates)
				if (update.id == id) {
					update.newMask[component] = false;
					// Commented out for now as objects in ComponentCollections may have obsolete masks
					// assert(update.newMask == updatedMaskForCheck);
					return;
				}

			auto updatedMask = currentMask;
			updatedMask[component] = false;
			assert(updatedMask == updatedMaskForCheck);

			updateMask(id, updatedMask);
		}

		void updateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask = false) {
			if (_updatesLocked == 0)
				doUpdateMask(id, newMask, ignoreOldMask);
			else
				_updates.push_back({ id, newMask, ignoreOldMask });
		}

		void doAllUpdates() {
			for (const auto & update : _updates)
				doUpdateMask(update.id, update.newMask, update.ignoreOldMask);
			_updates.clear();

			for (const auto removal : _removals)
				doRemove(removal.id);
			_removals.clear();
		}

		void doUpdateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask) {
			const auto oldMask = ignoreOldMask ? 0 : _entities[id];

			int done = 0;
			if (oldMask == 0) // Will not have to remove
				++done;

			for (auto & collection : _archetypes) {
				if (collection.mask == oldMask) {
					const auto size = collection.entities.size();
					if (size > 1) {
						const auto it = std::find(collection.entities.begin(), collection.entities.end(), id);
						std::iter_swap(it, collection.entities.begin() + size - 1);
					}
					collection.entities.pop_back();
					collection.sorted = false;
					++done;
				}

				if (collection.mask == newMask) {
					collection.entities.emplace_back(id);
					collection.sorted = false;
					++done;
				}

				if (done == 2)
					break;
			}

			if (done == 1)
				_archetypes.emplace_back(newMask, id);

			_entities[id] = newMask;
		}

		void doRemove(Entity::ID id) {
			auto & mask = _entities[id];

			SystemManager::removeEntity(EntityView(id, mask));

			for (auto & collection : _archetypes) {
				if (collection.mask == mask) {
					const auto tmp = std::find(collection.entities.begin(), collection.entities.end(), id);
					if (collection.entities.size() > 1) {
						std::swap(*tmp, collection.entities.back());
						collection.sorted = false;
					}
					collection.entities.pop_back();
					break;
				}
			}

			mask = 0;

			_toReuse.emplace_back(id);
			_toReuseSorted = false;
		}


	private:
		std::vector<Entity::Mask> _entities;
		std::vector<Archetype> _archetypes;
		std::vector<Entity::ID> _toReuse;
		bool _toReuseSorted = true;

		struct Update {
			Entity::ID id;
			Entity::Mask newMask;
			bool ignoreOldMask;
		};
		std::vector<Update> _updates;

		struct Removal {
			Entity::ID id;
		};
		std::vector<Removal> _removals;
		std::atomic<size_t> _updatesLocked = 0;

	private:
		detail::GlobalCompMap _components;

	public: // Reserved to systems
		detail::GlobalCompMap & __getComponentMap() { return _components; }
	};
}
