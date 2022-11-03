#include "registerDetachFrom.hpp"

// kengine meta
#include "meta/DetachFrom.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerDetachFrom() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::DetachFrom, Comps...>(
			[](const auto t, Entity & e) noexcept {
				using Type = putils_wrapped_type(t);
				e.detach<Type>();
			}
		);
	}
}

