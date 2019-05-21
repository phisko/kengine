#pragma once

#ifndef KENGINE_MAX_SAVE_PATH_LENGTH
# define KENGINE_MAX_SAVE_PATH_LENGTH 64
#endif

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
		Entity getEntity(Entity::ID id);
		EntityView getEntity(Entity::ID id) const;

    public:
		void removeEntity(EntityView e);
		void removeEntity(Entity::ID id);

    public:
		void load(const char * directory = ".");

		void save(const char * directory = ".") const;


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
				EntityIterator & operator++();
				bool operator!=(const EntityIterator & rhs) const;
				Entity operator*() const;

				size_t index;
				EntityManager & em;
			};

			EntityIterator begin() const;
			EntityIterator end() const;

			EntityCollection(EntityManager & em);
			~EntityCollection();

			EntityManager & em;
		};

	public:
		EntityCollection getEntities();

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
		Entity alloc();

    private:
		friend class Entity;
		void addComponent(Entity::ID id, size_t component, Entity::Mask updatedMaskForCheck);
		void removeComponent(Entity::ID id, size_t component, Entity::Mask updatedMaskForCheck);
		void updateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask = false);

		void doAllUpdates();
		void doUpdateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask);
		void doRemove(Entity::ID id);

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
