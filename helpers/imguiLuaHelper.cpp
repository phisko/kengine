#include "ImGuiHelper.hpp"
#include "kengine.hpp"
#include "data/LuaStateComponent.hpp"

extern lua_State * lState;
extern void LoadImguiBindings();

namespace kengine::imguiLuaHelper {
	void initBindings() noexcept {
		for (const auto & [e, state] : entities.with<LuaStateComponent>()) {
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}