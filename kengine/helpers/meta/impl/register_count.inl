#include "register_count.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine meta
#include "kengine/meta/count.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_count(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::count, Comps...>(
			r, [&](const auto t) noexcept {
				using type = putils_wrapped_type(t);
				return r.view<type>().size();
			}
		);
	}
}
