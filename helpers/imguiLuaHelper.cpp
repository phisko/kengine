#ifdef KENGINE_LUA

#include "imguiLuaHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "imgui_lua_bindings/imgui_lua_bindings.hpp"

// kengine data
#include "data/LuaStateComponent.hpp"

// kengine helpers
#include "helpers/imguiHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::imguiLuaHelper {
	void initBindings(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		for (const auto & [e, state] : r.view<LuaStateComponent>().each()) {
			kengine_logf(r, Log, "InitImguiLuaBindings", "Initializing bindings for state Entity %zu", e);
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}

#endif