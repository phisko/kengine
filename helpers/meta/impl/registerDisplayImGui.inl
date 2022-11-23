#include "registerDisplayImGui.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "meta/DisplayImGui.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace putils::reflection {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void imguiEdit(const char * name, const entt::entity & obj) noexcept {
		const auto nonEnumValue = static_cast<std::underlying_type_t<entt::entity>>(obj);
		imguiEdit(name, nonEnumValue);
	}
}

namespace kengine {
	template<typename ... Comps>
	void registerDisplayImGui(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::DisplayImGui, Comps...>(
			r, [&](const auto t, entt::const_handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Comp = putils_wrapped_type(t);
				const Comp * comp = nullptr;

				if constexpr (!std::is_empty<Comp>())
					comp = e.try_get<Comp>();
				else {
					static constexpr Comp instance;
					comp = &instance;
				}

				if (comp)
					putils::reflection::imguiEdit(*comp);
			}
		);
	}
}

