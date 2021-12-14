#include "kengine.hpp"
#include "remove_if.hpp"
#include "impl/types/registerTypes.hpp"
#include "functions/OnTerminate.hpp"

namespace kengine {
	Entities entities;

	void init(size_t threads) noexcept {
		impl::state = new impl::GlobalState(threads);

#ifndef KENGINE_NO_TYPE_REGISTRATION
		impl::types::registerTypes();
#endif
	}

	void * getState() noexcept {
		return impl::state;
	}

	void initPlugin(void * state) noexcept {
		impl::state = (impl::GlobalState *)state;
	}

	void terminate() noexcept {
		for (const auto & [e, func] : entities.with<functions::OnTerminate>())
			func();
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

	void cleanupArchetypes() noexcept {
		impl::WriteLock l(impl::state->_archetypesMutex);

		putils::remove_if(impl::state->_archetypes, [](const impl::Archetype & archetype) noexcept {
			return archetype.entities.empty();
		});
	}
}