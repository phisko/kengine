#include "register_edit_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine meta
#include "kengine/meta/for_each_entity.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename ... Comps>
	void register_for_each_entity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		register_meta_component_implementation<meta::for_each_entity, Comps...>(
			r, [&](const auto t, auto && func) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);
				for (const auto e : r.view<type>())
					func({ r, e });
			}
		);

		register_meta_component_implementation<meta::for_each_entity_without, Comps...>(
			r, [&](const auto t, auto && func) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);
				r.each([&](entt::entity e) {
					if (!r.all_of<type>(e))
						func({ r, e });
				});
			}
		);
	}
}

