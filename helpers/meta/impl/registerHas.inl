#include "registerHas.hpp"

// kengine meta
#include "meta/Has.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerHas() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::Has, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using T = putils_wrapped_type(t);
				return e.has<T>();
			}
		);
	}
}

