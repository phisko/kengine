#include "kengine.hpp"

namespace kengine {
	Entities entities;

	void init(size_t threads) noexcept {
		impl::state = new impl::GlobalState(threads);
	}

	void * getState() noexcept {
		return impl::state;
	}

	void initPlugin(void * state) noexcept {
		impl::state = (impl::GlobalState *)state;
	}

	void terminate() noexcept {
		delete impl::state;
	}

	putils::ThreadPool & threadPool() noexcept {
		return impl::state->threadPool;
	}

	bool isRunning() noexcept {
		return impl::state->running;
	}

	void stopRunning() noexcept {
		impl::state->running = false;
	}
}