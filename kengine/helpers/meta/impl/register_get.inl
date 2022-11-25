#include "register_get.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/get.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename ... Comps>
	void register_get(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::get, Comps...>(
			r, [&](const auto t, entt::handle e) noexcept -> void * {
				using type = putils_wrapped_type(t);
				if constexpr (!std::is_empty<type>())
					return &e.get<type>();
				else {
					static type instance;
					return &instance;
				}
			}
		);

		register_meta_component_implementation<meta::get_const, Comps...>(
			r, [](const auto t, entt::const_handle e) noexcept -> const void * {
				using type = putils_wrapped_type(t);
				if constexpr (!std::is_empty<type>())
					return &e.get<type>();
				else {
					static const type instance;
					return &instance;
				}
			}
		);
	}
}