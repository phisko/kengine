#pragma once

#include <vector>
#include "Component.hpp"
#include "Entity.hpp"
#include "ThreadPool.hpp"
#include "EntityCreator.hpp"
#include "functions/OnEntityCreated.hpp"

namespace kengine {
	template<typename T>
	struct no {
		using CompType = T;
	};

	template<typename>
	struct is_not : std::false_type {};

	template<typename T>
	struct is_not<no<T>> : std::true_type {};

    class EntityManager : public putils::ThreadPool {
    public:
		EntityManager(size_t threads = 0) : ThreadPool(threads) {
			detail::components = &_components;
		}
		~EntityManager();

    public:
		template<typename Func> // Func: kengine::EntityCreator
        Entity createEntity(Func && postCreate) {
			auto e = alloc();
			postCreate(e);

			for (const auto & [_, f] : getEntities<functions::OnEntityCreated>())
				f(e);

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

	public:
		std::atomic<bool> running = true;

	private:
		struct Archetype {
			Entity::Mask mask;
			std::vector<Entity::ID> entities;
			bool sorted = true;
			mutable detail::Mutex mutex;

			Archetype(Entity::Mask mask, Entity::ID firstEntity);
			Archetype() = default;
			Archetype(Archetype &&);
			Archetype(const Archetype &);

			void add(Entity::ID id);
			void remove(Entity::ID id);
			void sort();

			template<typename ... Comps>
			bool matches() {
				{
					detail::ReadLock l(mutex);
					if (entities.empty())
						return false;
				}

				bool good = true;
				putils::for_each_type<Comps...>([&](auto && type) {
					using T = putils_wrapped_type(type);

					if constexpr (kengine::is_not<T>()) {
						using CompType = typename T::CompType;
						const bool hasComp = mask.test(Component<CompType>::id());
						good &= !hasComp;
					}
					else {
						const bool hasComp = mask.test(Component<T>::id());
						good &= hasComp;
					}
				});

				if (good && !sorted) {
					detail::WriteLock l(mutex);
					sort();
				}

				return good;
			}
		};

	public:
		struct EntityCollection {
			struct Iterator {
				using iterator_category = std::forward_iterator_tag;
				using value_type = Entity;
				using reference = value_type &;
				using pointer = value_type *;
				using difference_type = size_t;

				Iterator & operator++();
				bool operator!=(const Iterator & rhs) const;
				bool operator==(const Iterator & rhs) const;
				Entity operator*() const;

				Iterator(size_t index, EntityManager & em) : index(index), em(em) {}
				Iterator(const Iterator &) = default;
				Iterator & operator=(const Iterator & rhs) {
					index = rhs.index; return *this;
				}

				size_t index;
				EntityManager & em;
			};

			using value_type = Iterator::value_type;
			using reference = Iterator::reference;
			using iterator = Iterator;

			Iterator begin() const;
			Iterator end() const;

			EntityManager & em;

			putils_reflection_class_name(EntityManagerEntityCollection);
		};

		EntityCollection getEntities();

		template<typename ... Comps>
		struct ComponentCollection {
			struct Iterator {
				using iterator_category = std::forward_iterator_tag;
				using value_type = std::tuple<Entity, Comps & ...> &;
				using reference = value_type &;
				using pointer = std::tuple<Entity, Comps & ...> *;
				using difference_type = size_t;

				Iterator & operator++() {
					++currentEntity;
					auto & archetype = em._archetypes[currentType];

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
						if (currentType >= em._archetypes.size())
							currentType = (size_t)-1;
					}

					return *this;
				}

				// Use `<` as it will only be compared with `end()`, and there is a risk that new entities have been added since `end()` was called
				bool operator!=(const Iterator & rhs) const { return currentType < rhs.currentType || currentEntity < rhs.currentEntity; }
				bool operator==(const Iterator & rhs) const { return !(*this != rhs); }

				template<typename T>
				static T & get(Entity & e) {
					if constexpr (kengine::is_not<T>()) {
						static T ret;
						return ret;
					}
					else
						return e.get<T>();
				};

				std::tuple<Entity, Comps &...> operator*() const {
					detail::ReadLock l(em._archetypesMutex);
					const auto & archetype = em._archetypes[currentType];

					detail::ReadLock l2(archetype.mutex);
					Entity e(archetype.entities[currentEntity], archetype.mask, &em);
					return std::make_tuple(e, std::ref(get<Comps>(e))...);
				}

				Iterator(EntityManager & em, size_t currentType, size_t currentEntity) : em(em), currentType(currentType), currentEntity(currentEntity) {}
				Iterator(const Iterator &) = default;
				Iterator & operator=(const Iterator & rhs) {
					currentType = rhs.currentType;
					currentEntity = rhs.currentEntity;
					return *this;
				}

				EntityManager & em;
				size_t currentType;
				size_t currentEntity;
			};

			using value_type = typename Iterator::value_type;
			using reference = typename Iterator::reference;
			using iterator = Iterator;

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
								return Iterator{ em, archetypeIndex, entityIndex };
							++entityIndex;
						}
					}
					++archetypeIndex;
				}

				return Iterator{ em, (size_t)-1, 0 };
			}

			auto end() const {
				detail::ReadLock l(em._archetypesMutex);
				return Iterator{ em, (size_t)-1, 0 };
			}

			EntityManager & em;

			template<typename T>
			static std::string getClassName() {
				return putils::reflection::get_class_name<T>();
			}
			static const auto reflection_get_class_name() {
				static std::string ret("EntityManagerComponentCollection" + (getClassName<Comps>() + ...));
				return ret.c_str();
			}
		};

		template<typename ... Comps>
		auto getEntities() {
			return ComponentCollection<Comps...>{ *this };
		}

	private:
		Entity alloc();

    private:
		friend class Entity;
		void addComponent(Entity::ID id, size_t component);
		void removeComponent(Entity::ID id, size_t component);
		void updateHasComponent(Entity::ID id, size_t component, bool newHasComponent);

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

	private:
		mutable detail::GlobalCompMap _components; // Mutable to lock mutex

	public: // Reserved to PluginHelper::initPlugin
		detail::GlobalCompMap & _getComponentMap() { return _components; }
	};
}
