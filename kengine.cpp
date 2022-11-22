#include "kengine.hpp"

// putils
#include "remove_if.hpp"

// kengine functions
#include "functions/OnTerminate.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine {
	Entities entities;

	void init(size_t threads) noexcept {
		KENGINE_PROFILING_SCOPE;

        delete impl::state;
		impl::state = new impl::GlobalState(threads);
	}

	void * getState() noexcept {
		return impl::state;
	}

	void initPlugin(void * state) noexcept {
		impl::state = (impl::GlobalState *)state;
	}

	void terminate() noexcept {
		KENGINE_PROFILING_SCOPE;

		for (const auto & [e, func] : entities.with<functions::OnTerminate>())
			func();
        delete impl::state;
		impl::state = nullptr;
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
		KENGINE_PROFILING_SCOPE;

		impl::WriteLock l(impl::state->_archetypesMutex);

		putils::remove_if(impl::state->_archetypes, [](const impl::Archetype & archetype) noexcept {
			return archetype.entities.empty();
		});
	}
}

namespace putils {
	template<>
	std::string toString(const entt::entity & entity) noexcept {
		const auto nonEnumValue = static_cast<std::underlying_type_t<entt::entity>>(entity);
		return toString(nonEnumValue);
	}

	template<>
	entt::entity parse(std::string_view str) noexcept {
		const auto nonEnumValue = parse<std::underlying_type_t<entt::entity>>(str);
		return static_cast<entt::entity>(nonEnumValue);
	}
}