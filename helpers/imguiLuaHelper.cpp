#ifdef KENGINE_LUA

#include "kengine.hpp"

// kengine data
#include "data/LuaStateComponent.hpp"

// kengine helpers
#include "helpers/imguiHelper.hpp"
#include "helpers/logHelper.hpp"

extern lua_State * lState;
extern void LoadImguiBindings();

namespace kengine::imguiLuaHelper {
	void initBindings() noexcept {
		for (const auto & [e, state] : entities.with<LuaStateComponent>()) {
			kengine_logf(Log, "InitImguiLuaBindings", "Initializing bindings for state Entity %zu", e.id);
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}

#endif