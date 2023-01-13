#include "register_edit_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/edit_imgui.hpp"

// kengine helpers
#include "kengine/helpers/imgui_edit_entity.hpp"
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_edit_imgui(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::edit_imgui, Comps...>(
			r, [](const auto t, entt::handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);
				type * comp = nullptr;

				if constexpr (!std::is_empty<type>())
					comp = e.try_get<type>();
				else {
					static type instance;
					comp = &instance;
				}

				if (comp)
					putils::reflection::imgui_edit(*comp);
			}
		);
	}
}
