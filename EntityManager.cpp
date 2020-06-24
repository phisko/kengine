#include <filesystem>
#include "EntityManager.hpp"

#include "functions/OnTerminate.hpp"
#include "functions/OnEntityRemoved.hpp"

#ifndef KENGINE_MAX_SAVE_PATH_LENGTH
# define KENGINE_MAX_SAVE_PATH_LENGTH 64
#endif

#ifndef KENGINE_MAX_COMPONENT_NAME_LENGTH
# define KENGINE_MAX_COMPONENT_NAME_LENGTH 64
#endif

namespace kengine {
	EntityManager::~EntityManager() {
		for (const auto & [e, func] : getEntities<functions::OnTerminate>())
			func();
	}

	Entity EntityManager::getEntity(Entity::ID id) {
		detail::ReadLock l(_entitiesMutex);
		return Entity(id, _entities[id].mask, this);
	}

	EntityView EntityManager::getEntity(Entity::ID id) const {
		detail::ReadLock l(_entitiesMutex);
		return EntityView(id, _entities[id].mask);
	}

	void EntityManager::removeEntity(EntityView e) {
		removeEntity(e.id);
	}

	void EntityManager::removeEntity(Entity::ID id) {
		auto e = getEntity(id);
		for (const auto & [_, func] : getEntities<functions::OnEntityRemoved>())
			func(e);

		{
			detail::ReadLock l(detail::components->mutex);
			for (const auto & [_, comp] : detail::components->map)
				if (e.componentMask[comp->id])
					comp->reset(id);
		}

		Entity::Mask mask;
		{
			detail::ReadLock entities(_entitiesMutex);
			mask = _entities[id].mask;
		}

		if (mask != 0) {
			detail::ReadLock archetypes(_archetypesMutex);
			const auto archetype = std::find_if(
				_archetypes.begin(), _archetypes.end(),
				[mask](const auto & a) { return a.mask == mask; }
			);
			archetype->remove(id);
		}

		{
			detail::WriteLock entities(_entitiesMutex);
			_entities[id].mask = 0;
			_entities[id].active = false;
			_entities[id].shouldActivateAfterInit = true;
		}

		detail::WriteLock l(_toReuseMutex);
		_toReuse.emplace_back(id);
		_toReuseSorted = false;
	}

	void EntityManager::setEntityActive(EntityView e, bool active) {
		setEntityActive(e.id, active);
	}

	void EntityManager::setEntityActive(Entity::ID id, bool active) {
		detail::WriteLock l(_entitiesMutex);
		_entities[id].active = active;
		_entities[id].shouldActivateAfterInit = active;
	}

	struct ComponentIDSave {
		size_t id;
		putils::string<KENGINE_MAX_COMPONENT_NAME_LENGTH> name;
	};

	Entity EntityManager::alloc() {
		{
			detail::ReadLock l(_toReuseMutex);
			if (_toReuse.empty()) {
				detail::WriteLock l(_entitiesMutex);
				const auto id = _entities.size();
				_entities.push_back({ false, 0 });
				return Entity(id, 0, this);
			}
		}

		Entity::ID id;
		{
			detail::WriteLock l(_toReuseMutex);
			if (!_toReuseSorted) {
				std::sort(_toReuse.begin(), _toReuse.end(), std::greater<Entity::ID>());
				_toReuseSorted = true;
			}
			id = _toReuse.back();
			_toReuse.pop_back();
		}

#ifndef KENGINE_NDEBUG
		{
			detail::ReadLock l(_archetypesMutex);
			for (const auto & archetype : _archetypes) {
				detail::ReadLock l(archetype.mutex);
				assert(std::find(archetype.entities.begin(), archetype.entities.end(), id) == archetype.entities.end());
			}
		}

		{
			detail::ReadLock l(_entitiesMutex);
			assert(id < _entities.size());
		}
#endif

		return Entity(id, 0, this);
	}

	void EntityManager::addComponent(Entity::ID id, size_t component) {
		updateHasComponent(id, component, true);
	}

	void EntityManager::removeComponent(Entity::ID id, size_t component) {
		updateHasComponent(id, component, false);
	}

	void EntityManager::updateHasComponent(Entity::ID id, size_t component, bool newHasComponent) {
		Entity::Mask oldMask;
		{
			detail::ReadLock l(_entitiesMutex);
			oldMask = _entities[id].mask;
		}

		auto updatedMask = oldMask;
		updatedMask[component] = newHasComponent;

		assert(oldMask != updatedMask);

		if (oldMask != 0) {
			detail::ReadLock l(_archetypesMutex);

			const auto archetype = std::find_if(
				_archetypes.begin(), _archetypes.end(),
				[oldMask](const auto & a) { return a.mask == oldMask; }
			);
			archetype->remove(id);
		}

		if (updatedMask != 0) {
			detail::WriteLock l(_archetypesMutex);

			const auto archetype = std::find_if(
				_archetypes.begin(), _archetypes.end(),
				[updatedMask](const auto & a) { return a.mask == updatedMask; }
			);
			if (archetype != _archetypes.end())
				archetype->add(id);
			else
				_archetypes.emplace_back(updatedMask, id);
		}

		detail::WriteLock l(_entitiesMutex);
		_entities[id].mask = updatedMask;
	}

	/*
	** Collection
	*/

	EntityManager::EntityCollection EntityManager::getEntities() {
		return EntityCollection{ *this };
	}

	EntityManager::EntityCollection::Iterator & EntityManager::EntityCollection::Iterator::operator++() {
		++index;
		detail::ReadLock l(em._entitiesMutex);
		while (index < em._entities.size() && (em._entities[index].mask == 0 || !em._entities[index].active))
			++index;
		return *this;
	}

	bool EntityManager::EntityCollection::Iterator::operator!=(const Iterator & rhs) const {
		// Use `<` as it will only be compared with `end()`, and there is a risk that new entities have been added since `end()` was called
		return index < rhs.index;
	}

	bool EntityManager::EntityCollection::Iterator::operator==(const Iterator & rhs) const {
		return !(*this != rhs);
	}


	Entity EntityManager::EntityCollection::Iterator::operator*() const {
		detail::ReadLock l(em._entitiesMutex);
		return Entity(index, em._entities[index].mask, &em);
	}

	EntityManager::EntityCollection::Iterator EntityManager::EntityCollection::begin() const {
		size_t i = 0;
		detail::ReadLock l(em._entitiesMutex);
		while (i < em._entities.size() && (em._entities[i].mask == 0 || !em._entities[i].active))
			++i;
		return { i, em };
	}

	EntityManager::EntityCollection::Iterator EntityManager::EntityCollection::end() const {
		detail::ReadLock l(em._entitiesMutex);
		return { em._entities.size(), em };
	}

	/*
	** Archetype
	*/

	EntityManager::Archetype::Archetype(Entity::Mask mask, Entity::ID firstEntity)
		: mask(mask), entities({ firstEntity })
	{}

	EntityManager::Archetype::Archetype(Archetype && rhs) {
		mask = rhs.mask;
		sorted = rhs.sorted;
		detail::WriteLock l(rhs.mutex);
		entities = std::move(rhs.entities);
	}

	EntityManager::Archetype::Archetype(const Archetype & rhs) {
		mask = rhs.mask;
		sorted = rhs.sorted;
		detail::ReadLock l(rhs.mutex);
		entities = rhs.entities;
	}

	void EntityManager::Archetype::add(Entity::ID id) {
		detail::WriteLock l(mutex);
		entities.push_back(id);
		sorted = false;
	}

	void EntityManager::Archetype::remove(Entity::ID id) {
		detail::WriteLock l(mutex);
		if (!sorted)
			sort();
		const auto it = std::lower_bound(entities.begin(), entities.end(), id);
		std::swap(*it, entities.back());
		entities.pop_back();
		sorted = false;
	}

	void EntityManager::Archetype::sort() {
		std::sort(entities.begin(), entities.end(), std::less<Entity::ID>());
		sorted = true;
	}
}