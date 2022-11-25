#include "register_has.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/has.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename ... Comps>
	void register_has(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::has, Comps...>(
			r, [](const auto t, entt::const_handle e) noexcept {
				using type = putils_wrapped_type(t);
				return e.all_of<type>();
			}
		);
	}
}

