#include "register_display_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "kengine/meta/display_imgui.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace putils::reflection {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void imgui_edit(const char * name, const entt::entity & obj) noexcept {
		const auto non_enum_value = static_cast<std::underlying_type_t<entt::entity>>(obj);
		imgui_edit(name, non_enum_value);
	}
}

namespace kengine {
	template<typename ... Comps>
	void register_display_imgui(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::display_imgui, Comps...>(
			r, [&](const auto t, entt::const_handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);
				const type * comp = nullptr;

				if constexpr (!std::is_empty<type>())
					comp = e.try_get<type>();
				else {
					static constexpr type instance;
					comp = &instance;
				}

				if (comp)
					putils::reflection::imgui_edit(*comp);
			}
		);
	}
}

