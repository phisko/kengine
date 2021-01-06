#include <algorithm>

#include "GlobalState.hpp"
#include "kengine.hpp"
#include "functions/OnTerminate.hpp"

namespace kengine::impl {
	GlobalState * state = nullptr;

	GlobalState::GlobalState(size_t threads) noexcept
		: threadPool(threads)
	{}

	GlobalState::~GlobalState() noexcept {
		for (const auto & [e, func] : entities.with<functions::OnTerminate>())
			func();
	}

	Entity alloc() noexcept {
		{
			ReadLock l(state->_toReuseMutex);
			if (state->_toReuse.empty()) {
				WriteLock l(state->_entitiesMutex);
				const auto id = state->_entities.size();
				state->_entities.push_back({ false, 0 });
				return { id, 0 };
			}
		}

		EntityID id;
		{
			WriteLock l(state->_toReuseMutex);
			if (!state->_toReuseSorted) {
				std::sort(state->_toReuse.begin(), state->_toReuse.end(), std::greater<ID>());
				state->_toReuseSorted = true;
			}
			id = state->_toReuse.back();
			state->_toReuse.pop_back();
		}

#ifndef KENGINE_NDEBUG
		{
			ReadLock l(state->_archetypesMutex);
			for (const auto & archetype : state->_archetypes) {
				ReadLock l(archetype.mutex);
				assert(std::find(archetype.entities.begin(), archetype.entities.end(), id) == archetype.entities.end());
			}
		}

		{
			ReadLock l(state->_entitiesMutex);
			assert(id < state->_entities.size());
		}
#endif

		return Entity(id, 0);
	}
}