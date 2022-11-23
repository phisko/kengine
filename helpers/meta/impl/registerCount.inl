#include "registerCount.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine meta
#include "meta/Count.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerCount(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::Count, Comps...>(
			r, [&](const auto t) noexcept {
				using Type = putils_wrapped_type(t);
				return r.view<Type>().size();
			}
		);
	}
}
