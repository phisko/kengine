#pragma once

#include "impl/GlobalState.hpp"
#include "Entities.hpp"

namespace kengine {
	void init(size_t threads = 0) noexcept;

#ifdef KENGINE_TYPE_REGISTRATION
	namespace types {
		void registerTypes() noexcept;
	}
#endif

	class Entities;
	extern Entities entities;

	putils::ThreadPool & threadPool() noexcept;
	bool isRunning() noexcept;
	void stopRunning() noexcept;

	void terminate() noexcept;

	void * getState() noexcept;
	void initPlugin(void * state) noexcept;

	void cleanupArchetypes() noexcept;
}
