#pragma once

#include <atomic>
#include <vector>
#include <memory>

#include "impl/Mutex.hpp"
#include "impl/ComponentMask.hpp"
#include "impl/ComponentMetadata.hpp"
#include "ThreadPool.hpp"

namespace kengine {
    class Entity;
}

namespace kengine::impl {
	struct GlobalState {
		GlobalState(size_t threads) noexcept;

		putils::ThreadPool threadPool;
		std::atomic<bool> running = true;

		/*
		Impl
		*/

		union EntityMetadata {
			struct {
				bool active;
				ComponentMask mask;
				bool shouldActivateAfterInit;
			};
			ID freeListNext;
		};
		std::vector<EntityMetadata> _entities;
		Mutex _entitiesMutex;

		std::vector<struct Archetype> _archetypes;
		Mutex _archetypesMutex;

		ID _freeList = INVALID_ID;
		Mutex _freeListMutex;

		std::vector<std::unique_ptr<ComponentMetadata>> _components;
		Mutex _componentsMutex;
	};

	Entity alloc() noexcept;
	extern GlobalState * state;
}