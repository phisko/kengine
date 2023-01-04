#include "register_remove.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/remove.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_remove(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::remove, Comps...>(
			r, [](const auto t, entt::handle e) noexcept {
				using type = putils_wrapped_type(t);
				e.remove<type>();
			}
		);
	}
}
