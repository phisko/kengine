#include "Entities.hpp"

// meta
#include "meta/fwd.hpp"

// kengine functions
#include "functions/OnEntityCreated.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

// kengine impl
#include "impl/GlobalState.hpp"

namespace kengine {
	template<typename Func>
	Entity Entities::create(Func && postCreate) noexcept {
		KENGINE_PROFILING_SCOPE;

		auto e = impl::alloc();
		postCreate(e);

		for (const auto [_, f] : with<functions::OnEntityCreated>())
			f(e);

		bool shouldActivate;
		{
			impl::ReadLock l(impl::state->_entitiesMutex);
			shouldActivate = impl::state->_entities[e.id].data.shouldActivateAfterInit;
		}
		if (shouldActivate)
			setActive(e, true);
		return e;
	}

	template<typename Func>
	Entity Entities::operator+=(Func && postCreate) noexcept {
		return create(FWD(postCreate));
	}

	template<typename ... Comps>
	impl::ComponentCollection<Comps...> Entities::with() const noexcept {
		KENGINE_PROFILING_SCOPE;
		return impl::ComponentCollection<Comps...>{};
	}
}