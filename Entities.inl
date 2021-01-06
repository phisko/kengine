#include "Entities.hpp"
#include "functions/OnEntityCreated.hpp"
#include "fwd.hpp"

namespace kengine {
	template<typename Func>
	Entity Entities::create(Func && postCreate) noexcept {
		auto e = impl::alloc();
		postCreate(e);

		for (const auto [_, f] : with<functions::OnEntityCreated>())
			f(e);

		bool shouldActivate;
		{
			impl::ReadLock l(impl::state->_entitiesMutex);
			shouldActivate = impl::state->_entities[e.id].shouldActivateAfterInit;
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
		return impl::ComponentCollection<Comps...>{};
	}
}