#pragma once

#include "impl/GlobalState.hpp"
#include "Entity.hpp"
#include "Entities.hpp"

namespace kengine {
	void init(size_t threads = 0) noexcept;

	class Entities;
	extern Entities entities;

	putils::ThreadPool & threadPool() noexcept;
	bool isRunning() noexcept;
	void stopRunning() noexcept;

	void terminate() noexcept;

	void * getState() noexcept;
	void initPlugin(void * state) noexcept;
}
