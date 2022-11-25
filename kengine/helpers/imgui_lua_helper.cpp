#ifdef KENGINE_LUA

#include "imgui_lua_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/imgui_lua_bindings/imgui_lua_bindings.hpp"

// kengine data
#include "kengine/data/lua_state.hpp"

// kengine helpers
#include "kengine/helpers/imgui_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::imgui_lua_helper {
	void init_bindings(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		for (const auto & [e, state] : r.view<data::lua_state>().each()) {
			kengine_logf(r, log, "InitImguiLuaBindings", "Initializing bindings for state Entity %zu", e);
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}

#endif