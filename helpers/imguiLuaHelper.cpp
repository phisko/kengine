#ifdef KENGINE_LUA

#include "imguiLuaHelper.hpp"
#include "kengine.hpp"

// putils
#include "imgui_lua_bindings/imgui_lua_bindings.hpp"

// kengine data
#include "data/LuaStateComponent.hpp"

// kengine helpers
#include "helpers/imguiHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::imguiLuaHelper {
	void initBindings() noexcept {
		KENGINE_PROFILING_SCOPE;

		for (const auto & [e, state] : entities.with<LuaStateComponent>()) {
			kengine_logf(Log, "InitImguiLuaBindings", "Initializing bindings for state Entity %zu", e.id);
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}

#endif