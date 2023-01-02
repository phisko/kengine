#include "register_detach_from.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/detach_from.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_detach_from(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::detach_from, Comps...>(
			r, [](const auto t, entt::handle e) noexcept {
				using type = putils_wrapped_type(t);
				e.remove<type>();
			}
		);
	}
}
