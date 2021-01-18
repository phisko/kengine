#include "Entities.hpp"
#include "functions/OnEntityRemoved.hpp"
#include "impl/ComponentMask.hpp"

namespace kengine {
	Entity Entities::get(EntityID id) noexcept {
		impl::ReadLock l(impl::state->_entitiesMutex);
		return { id, impl::state->_entities[id].mask };
	}

	Entity Entities::operator[](EntityID id) noexcept {
		return get(id);
	}

	void Entities::remove(Entity e) noexcept {
		remove(e.id);
	}

	void Entities::remove(EntityID id) noexcept {
		auto e = get(id);

		for (const auto & [_, func] : with<functions::OnEntityRemoved>())
			func(e);

		size_t componentCount;
		{
			impl::ReadLock l(impl::state->_componentsMutex);
			componentCount = impl::state->_components.size();
		} // Avoid having the lock active while destroying components, as they might need to access new component types which would need to create new metadata
		for (size_t i = 0; i < componentCount; ++i)
			if (e.componentMask[i])
				impl::state->_components[i]->reset(id);

		impl::ComponentMask mask;
		{
			impl::ReadLock entities(impl::state->_entitiesMutex);
			mask = impl::state->_entities[id].mask;
		}

		if (mask != 0) {
			impl::ReadLock archetypes(impl::state->_archetypesMutex);
			const auto archetype = std::find_if(
				impl::state->_archetypes.begin(), impl::state->_archetypes.end(),
				[mask](const auto & a) noexcept { return a.mask == mask; }
			);
			archetype->remove(id);
		}

		{
			impl::WriteLock entities(impl::state->_entitiesMutex);
			auto & entityData = impl::state->_entities[id];
			entityData.mask = 0;
			entityData.active = false;
			entityData.shouldActivateAfterInit = true;
		}

		impl::WriteLock l(impl::state->_freeListMutex);
		impl::ReadLock entities(impl::state->_entitiesMutex);
		impl::state->_entities[id].freeListNext = impl::state->_freeList;
		impl::state->_freeList = id;
	}

	void Entities::operator-=(Entity e) noexcept {
		remove(e.id);
	}

	void Entities::operator-=(EntityID id) noexcept {
		remove(id);
	}

	void Entities::setActive(Entity e, bool active) noexcept {
		setActive(e.id, active);
	}

	void Entities::setActive(EntityID id, bool active) noexcept {
		impl::WriteLock l(impl::state->_entitiesMutex);
		impl::state->_entities[id].active = active;
		impl::state->_entities[id].shouldActivateAfterInit = active;
	}

	impl::EntityIterator Entities::begin() const noexcept {
		size_t i = 0;
		impl::ReadLock l(impl::state->_entitiesMutex);
		while (i < impl::state->_entities.size() && (impl::state->_entities[i].mask == 0 || !impl::state->_entities[i].active))
			++i;
		return { i };
	}

	impl::EntityIterator Entities::end() const noexcept {
		impl::ReadLock l(impl::state->_entitiesMutex);
		return { impl::state->_entities.size() };
	}
}