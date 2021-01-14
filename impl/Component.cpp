#include "Component.hpp"
#include "ComponentMask.hpp"
#include "helpers/assertHelper.hpp"

namespace kengine::impl {
	static void updateHasComponent(EntityID entity, ComponentID component, bool newHasComponent) noexcept {
		ComponentMask oldMask;
		{
			ReadLock l(state->_entitiesMutex);
			oldMask = state->_entities[entity].mask;
		}

		auto updatedMask = oldMask;
		updatedMask[component] = newHasComponent;

		kengine_assert(oldMask != updatedMask);

		if (oldMask != 0) {
			ReadLock l(state->_archetypesMutex);

			const auto archetype = std::find_if(
				state->_archetypes.begin(), state->_archetypes.end(),
				[oldMask](const auto & a) noexcept { return a.mask == oldMask; }
			);
			archetype->remove(entity);
		}

		if (updatedMask != 0) {
			WriteLock l(state->_archetypesMutex);

			const auto archetype = std::find_if(
				state->_archetypes.begin(), state->_archetypes.end(),
				[updatedMask](const auto & a) noexcept { return a.mask == updatedMask; }
			);
			if (archetype != state->_archetypes.end())
				archetype->add(entity);
			else
				state->_archetypes.emplace_back(updatedMask, entity);
		}

		WriteLock l(state->_entitiesMutex);
		state->_entities[entity].mask = updatedMask;
	}

	void addComponent(EntityID entity, ComponentID component) noexcept {
		updateHasComponent(entity, component, true);
	}

	void removeComponent(EntityID entity, ComponentID component) noexcept {
		updateHasComponent(entity, component, false);
	}
}