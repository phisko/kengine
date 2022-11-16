#pragma once

// kengine
#include "Entities.hpp"

// kengine impl
#include "impl/GlobalState.hpp"

#ifdef KENGINE_TYPE_REGISTRATION
#include "impl/types/registerTypes.hpp"
#endif

namespace kengine {
	KENGINE_CORE_EXPORT void init(size_t threads = 0) noexcept;

	class Entities;
	KENGINE_CORE_EXPORT extern Entities entities;

	KENGINE_CORE_EXPORT putils::ThreadPool & threadPool() noexcept;
	KENGINE_CORE_EXPORT bool isRunning() noexcept;
	KENGINE_CORE_EXPORT void stopRunning() noexcept;

	KENGINE_CORE_EXPORT void terminate() noexcept;

	KENGINE_CORE_EXPORT void * getState() noexcept;
	KENGINE_CORE_EXPORT void initPlugin(void * state) noexcept;

	KENGINE_CORE_EXPORT void cleanupArchetypes() noexcept;
}
