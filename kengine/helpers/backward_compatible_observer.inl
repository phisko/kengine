#include "backward_compatible_observer.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	backward_compatible_observer<Comps...>::backward_compatible_observer(entt::registry & r, const callback_type & callback) noexcept
		: r(r),
		  observer(r, entt::collector.group<Comps...>()),
		  callback(callback) {
		KENGINE_PROFILING_SCOPE;
		r.view<Comps...>().each(callback);
	}

	template<typename... Comps>
	void backward_compatible_observer<Comps...>::process() noexcept {
		KENGINE_PROFILING_SCOPE;
		for (const auto e : observer)
			callback(e, r.get<Comps>(e)...);
		observer.clear();
	}
}