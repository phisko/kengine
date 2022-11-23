#include "registerHas.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "meta/Has.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerHas(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::Has, Comps...>(
			r, [](const auto t, entt::const_handle e) noexcept {
				using T = putils_wrapped_type(t);
				return e.all_of<T>();
			}
		);
	}
}

