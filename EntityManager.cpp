#include "EntityManager.hpp"

namespace kengine {
	Entity EntityManager::getEntity(Entity::ID id) {
			return Entity(id, _entities[id], this);
	}

	EntityView EntityManager::getEntity(Entity::ID id) const {
		return EntityView(id, _entities[id]);
	}

	void EntityManager::removeEntity(EntityView e) {
		removeEntity(e.id);
	}

	void EntityManager::removeEntity(Entity::ID id) {
		if (_updatesLocked)
			_removals.push_back({ id });
		else
			doRemove(id);
	}

	void EntityManager::load(const char * directory) {
		_archetypes.clear();
		_toReuse.clear();

		for (size_t i = 0; i < _entities.size(); ++i)
			SystemManager::removeEntity(EntityView(i, _entities[i]));

		for (const auto &[_, meta] : _components)
			meta->load(directory);

		std::ifstream f(putils::string<KENGINE_MAX_SAVE_PATH_LENGTH>("%s/entities.bin", directory));
		assert(f);
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
			}
			else
				_toReuse.emplace_back(i);
		}

		SystemManager::load(directory);
	}

	void EntityManager::save(const char * directory) const {
		SystemManager::save(directory);

		std::vector<bool> serializable;

		serializable.resize(_components.size());
		for (const auto &[_, meta] : _components)
			serializable[meta->getId()] = meta->save(directory);

		std::ofstream f(putils::string<KENGINE_MAX_SAVE_PATH_LENGTH>("%s/entities.bin", directory));
		assert(f);
		const auto size = _entities.size();
		f.write((const char *)&size, sizeof(size));
		for (Entity::Mask mask : _entities) {
			for (size_t i = 0; i < serializable.size(); ++i)
				mask[i] = mask[i] & serializable[i];
			f.write((const char *)&mask, sizeof(mask));
		}
	}

	Entity EntityManager::alloc() {
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

	void EntityManager::addComponent(Entity::ID id, size_t component, Entity::Mask updatedMaskForCheck) {
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

	void EntityManager::removeComponent(Entity::ID id, size_t component, Entity::Mask updatedMaskForCheck) {
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

	void EntityManager::updateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask) {
		if (_updatesLocked == 0)
			doUpdateMask(id, newMask, ignoreOldMask);
		else
			_updates.push_back({ id, newMask, ignoreOldMask });
	}

	void EntityManager::doAllUpdates() {
		for (const auto & update : _updates)
			doUpdateMask(update.id, update.newMask, update.ignoreOldMask);
		_updates.clear();

		for (const auto removal : _removals)
			doRemove(removal.id);
		_removals.clear();
	}

	void EntityManager::doUpdateMask(Entity::ID id, Entity::Mask newMask, bool ignoreOldMask) {
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

	void EntityManager::doRemove(Entity::ID id) {
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

	/*
	** Collection
	*/

	EntityManager::EntityCollection EntityManager::getEntities() {
		return EntityCollection{ *this };
	}

	EntityManager::EntityCollection::EntityIterator & EntityManager::EntityCollection::EntityIterator::operator++() {
		++index;
		while (index < em._entities.size() && em._entities[index] == 0)
			++index;
		return *this;
	}

	bool EntityManager::EntityCollection::EntityIterator::operator!=(const EntityIterator & rhs) const {
		return index != rhs.index;
	}

	Entity EntityManager::EntityCollection::EntityIterator::operator*() const {
		return Entity(index, em._entities[index], &em);
	}

	EntityManager::EntityCollection::EntityIterator EntityManager::EntityCollection::begin() const {
		size_t i = 0;
		while (i < em._entities.size() && em._entities[i] == 0)
			++i;
		return EntityIterator{ i, em };
	}

	EntityManager::EntityCollection::EntityIterator EntityManager::EntityCollection::end() const {
		return EntityIterator{ em._entities.size(), em };
	}

	EntityManager::EntityCollection::EntityCollection(EntityManager & em) : em(em) {
		++em._updatesLocked;
	}

	EntityManager::EntityCollection::~EntityCollection() {
		assert(em._updatesLocked != 0);
		--em._updatesLocked;
		if (em._updatesLocked == 0)
			em.doAllUpdates();
	}
}