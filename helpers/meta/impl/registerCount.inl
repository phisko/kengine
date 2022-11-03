#include "registerCount.hpp"

// kengine meta
#include "meta/Count.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerCount() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::Count, Comps...>(
			[](const auto t) noexcept {
				using Type = putils_wrapped_type(t);
				return entities.with<Type>().size();
			}
		);
	}
}
