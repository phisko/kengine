#pragma once

// stl
#include <atomic>
#include <vector>
#include <memory>

// putils
#include "ThreadPool.hpp"

// kengine impl
#include "impl/Mutex.hpp"
#include "impl/ComponentMask.hpp"
#include "impl/ComponentMetadata.hpp"

namespace kengine {
    class Entity;
}

namespace kengine::impl {
	struct KENGINE_CORE_EXPORT GlobalState {
		GlobalState(size_t threads) noexcept;

		putils::ThreadPool threadPool;
		std::atomic<bool> running = true;

		/*
		Impl
		*/

		union KENGINE_CORE_EXPORT EntityMetadata {
			struct {
				bool active;
				ComponentMask mask;
				bool shouldActivateAfterInit;
			} data;
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

	KENGINE_CORE_EXPORT Entity alloc() noexcept;
	KENGINE_CORE_EXPORT extern GlobalState * state;
}