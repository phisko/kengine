#include "registerGet.hpp"

// kengine meta
#include "meta/Get.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerGet() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::Get, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using T = putils_wrapped_type(t);
				return (void *)&e.get<T>();
			}
		);
	}
}

