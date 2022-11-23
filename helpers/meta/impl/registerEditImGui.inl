#include "registerEditImGui.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "meta/EditImGui.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace putils::reflection {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void imguiEdit(const char * name, entt::entity & obj) noexcept {
		auto nonEnumValue = entt::id_type(obj);
		imguiEdit(name, nonEnumValue);
		obj = entt::entity(nonEnumValue);
	}
}

namespace kengine {
	template<typename ... Comps>
	void registerEditImGui(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::EditImGui, Comps...>(
			r, [](const auto t, entt::handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Comp = putils_wrapped_type(t);
				Comp * comp = nullptr;

				if constexpr (!std::is_empty<Comp>())
					comp = e.try_get<Comp>();
				else {
					static Comp instance;
					comp = &instance;
				}

				if (comp)
					putils::reflection::imguiEdit(*comp);
			}
		);
	}
}

