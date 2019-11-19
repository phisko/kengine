#pragma once

#include <vector>
#include "SystemManager.hpp"
#include "Component.hpp"
#include "Entity.hpp"

namespace kengine {
    class EntityManager : public SystemManager {
    public:
		EntityManager(size_t threads = 0) : SystemManager(threads) {
			detail::components = &_components;
		}
		~EntityManager();

    public:
		template<typename Func> // Func: void(Entity &);
        Entity createEntity(Func && postCreate) {
			auto e = alloc();
			postCreate(e);
			SystemManager::registerEntity(e);

			bool shouldActivate;
			{
				detail::ReadLock l(_entitiesMutex);
				shouldActivate = _entities[e.id].shouldActivateAfterInit;
			}
			if (shouldActivate)
				setEntityActive(e, true);
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
		void setEntityActive(EntityView e, bool active);
		void setEntityActive(Entity::ID id, bool active);

	private:
		struct Archetype {
			Entity::Mask mask;
			std::vector<Entity::ID> entities;
			bool sorted = true;
			mutable detail::Mutex mutex;

			Archetype(Entity::Mask mask, Entity::ID firstEntity) : mask(mask), entities({ firstEntity }) {}

			template<typename ... Comps>
			bool matches() {
				{
					detail::ReadLock l(mutex);
					if (entities.empty())
						return false;
				}

				if (!sorted) {
					detail::WriteLock l(mutex);
					std::sort(entities.begin(), entities.end(), std::greater<Entity::ID>());
					sorted = true;
				}

				bool good = true;
				putils_for_each_type(Comps, [&](auto && type) {
					using CompType = putils_wrapped_type(type);
					good &= mask.test(Component<CompType>::id());
				});
				return good;
			}

			Archetype() = default;
			Archetype(Archetype && rhs) = default;
			Archetype(const Archetype & rhs) {
				mask = rhs.mask;
				sorted = rhs.sorted;
				detail::ReadLock l(rhs.mutex);
				entities = rhs.entities;
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
					const auto & archetype = em._archetypes[currentType];

					{
						detail::ReadLock l(archetype.mutex);
						if (currentEntity < archetype.entities.size())
						{
							detail::ReadLock l(em._entitiesMutex);
							if (em._entities[archetype.entities[currentEntity]].active)
								return *this;
						}
					}

					currentEntity = 0;
					{
						detail::ReadLock l(em._archetypesMutex);
						for (++currentType; currentType < em._archetypes.size(); ++currentType)
							if (em._archetypes[currentType].matches<Comps...>())
								break;
					}

					return *this;
				}

				// Use `<` as it will only be compared with `end()`, and there is a risk that new entities have been added since `end()` was called
				bool operator!=(const ComponentIterator & rhs) const { return currentType < rhs.currentType || currentEntity < rhs.currentEntity; }

				std::tuple<Entity, Comps &...> operator*() const {
					detail::ReadLock l(em._archetypesMutex);
					const auto & archetype = em._archetypes[currentType];

					detail::ReadLock l2(archetype.mutex);
					Entity e(archetype.entities[currentEntity], archetype.mask, &em);
					return std::make_tuple(e, std::ref(e.get<Comps>())...);
				}

				kengine::EntityManager & em;
				size_t currentType;
				size_t currentEntity;
			};

			auto begin() const {
				detail::ReadLock l(em._archetypesMutex);

				size_t archetypeIndex = 0;
				for (auto & archetype : em._archetypes) {
					if (archetype.matches<Comps...>()) {
						detail::ReadLock l(archetype.mutex);
						detail::ReadLock l2(em._entitiesMutex);

						size_t entityIndex = 0;
						for (const auto entityID : archetype.entities) {
							if (em._entities[entityID].active)
								return ComponentIterator{ em, archetypeIndex, entityIndex };
							++entityIndex;
						}
					}
					++archetypeIndex;
				}

				return ComponentIterator{ em, em._archetypes.size(), 0 };
			}

			auto end() const {
				detail::ReadLock l(em._archetypesMutex);
				return ComponentIterator{ em, em._archetypes.size(), 0 };
			}

			ComponentCollection(EntityManager & em) : em(em) {
				detail::WriteLock l(em._updatesMutex);
				++em._updatesLocked;
			}

			~ComponentCollection() {
				detail::WriteLock l(em._updatesMutex);
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

	public:
		// In the following functions, `Func` must inherit from kengine::functions::BaseFunction, i.e. have:
		//		a `Signature` type alias for a function pointer type
		//		a `Signature funcPtr;` attribute

		template<typename Comp, typename Func>
		void registerComponentFunction(Func func) const {
			Component<Comp>::registerFunction(func);
		}

		using FunctionMapCollection = putils::vector<const FunctionMap *, KENGINE_COMPONENT_COUNT>;
		FunctionMapCollection getComponentFunctionMaps() const {
			FunctionMapCollection ret;

			{
				detail::ReadLock l(_components.mutex);
				for (const auto & p : _components.map)
					ret.push_back(&p.second->funcs);
			}

			return ret;
		}

	private:
		Entity alloc();

    private:
		friend class Entity;
		void addComponent(Entity::ID id, size_t component);
		void removeComponent(Entity::ID id, size_t component);
		void updateHasComponent(Entity::ID id, size_t component, bool newHasComponent);
		void updateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask = false);

		void doAllUpdates();
		void doUpdateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask);
		void doRemove(Entity::ID id);

	private:
		struct EntityMetadata {
			bool active = false;
			Entity::Mask mask = 0;
			bool shouldActivateAfterInit = true;
		};
		std::vector<EntityMetadata> _entities;
		mutable detail::Mutex _entitiesMutex;

		std::vector<Archetype> _archetypes;
		mutable detail::Mutex _archetypesMutex;

		std::vector<Entity::ID> _toReuse;
		bool _toReuseSorted = true;
		mutable detail::Mutex _toReuseMutex;

		struct Update {
			Entity::ID id;
			Entity::Mask newMask;
			bool ignoreOldMask;
			std::mutex mutex;

			Update() = default;
			Update(Update &&) = default;
			Update(const Update & rhs) {
				id = rhs.id;
				newMask = rhs.newMask;
				ignoreOldMask = rhs.ignoreOldMask;
			}
		};
		std::vector<Update> _updates;

		struct Removal {
			Entity::ID id;
		};
		std::vector<Removal> _removals;

		size_t _updatesLocked = 0;
		mutable detail::Mutex _updatesMutex;

	private:
		mutable detail::GlobalCompMap _components; // Mutable to lock mutex

	public: // Reserved to systems
		detail::GlobalCompMap & __getComponentMap() { return _components; }
	};
}
