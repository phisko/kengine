#include "GlobalState.hpp"
#include "kengine.hpp"

// stl
#include <algorithm>

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::impl {
	GlobalState * state = nullptr;

	GlobalState::GlobalState(size_t threads) noexcept
		: threadPool(threads)
	{}

	Entity alloc() noexcept {
		KENGINE_PROFILING_SCOPE;

		{
			ReadLock l(state->_freeListMutex);
			if (state->_freeList == INVALID_ID) {
				WriteLock l(state->_entitiesMutex);
				const auto id = state->_entities.size();
				state->_entities.push_back({ false, 0, true });
				return { id, 0 };
			}
		}

		EntityID id;
		{
			WriteLock l(state->_freeListMutex);
			id = state->_freeList;
			ReadLock entitiesLock(state->_entitiesMutex);
			state->_freeList = state->_entities[id].freeListNext;
		}

#ifndef KENGINE_NDEBUG
		{
			ReadLock l(state->_archetypesMutex);
			for (const auto & archetype : state->_archetypes) {
				ReadLock l(archetype.mutex);
				kengine_assert(std::find(archetype.entities.begin(), archetype.entities.end(), id) == archetype.entities.end());
			}
		}

		{
			ReadLock l(state->_entitiesMutex);
			kengine_assert(id < state->_entities.size());
		}
#endif

		return Entity(id, 0);
	}
}