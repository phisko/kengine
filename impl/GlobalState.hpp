#pragma once

#include <atomic>
#include <vector>
#include <unordered_map>
#include <memory>

#include "impl/Mutex.hpp"
#include "impl/Archetype.hpp"
#include "impl/ComponentMetadata.hpp"
#include "ThreadPool.hpp"

#include "meta/type.hpp"

namespace kengine {
	class Entity;
}

namespace kengine::impl {
	struct GlobalState {
		GlobalState(size_t threads) noexcept;
		~GlobalState() noexcept;

		putils::ThreadPool threadPool;
		std::atomic<bool> running = true;

		/*
		Impl
		*/

		struct EntityMetadata {
			bool active = false;
			ComponentMask mask = 0;
			bool shouldActivateAfterInit = true;
		};
		std::vector<EntityMetadata> _entities;
		mutable Mutex _entitiesMutex;

		std::vector<Archetype> _archetypes;
		mutable Mutex _archetypesMutex;

		std::vector<ID> _toReuse;
		bool _toReuseSorted = true;
		mutable Mutex _toReuseMutex;

		std::vector<std::unique_ptr<ComponentMetadata>> _components;
		Mutex _componentsMutex;
	};

	Entity alloc() noexcept;
	extern GlobalState * state;
}