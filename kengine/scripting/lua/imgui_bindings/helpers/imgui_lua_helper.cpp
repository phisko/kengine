#include "imgui_lua_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/imgui_lua_bindings/imgui_lua_bindings.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine scripting/lua
#include "kengine/scripting/lua/data/lua_state.hpp"

namespace kengine::imgui_lua_helper {
	void init_bindings(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "imgui_lua_bindings", "Initializing bindings");

		for (const auto & [e, state] : r.view<data::lua_state>().each()) {
			kengine_logf(r, verbose, "imgui_lua_bindings", "Initializing bindings for state entity [%u]", e);
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}