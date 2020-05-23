#include "ImGuiHelper.hpp"
#include "EntityManager.hpp"
#include "data/LuaStateComponent.hpp"

extern lua_State * lState;
extern void LoadImguiBindings();

namespace kengine::imguiLuaHelper {
	void initBindings(EntityManager & em) {

		for (const auto & [e, state] : em.getEntities<LuaStateComponent>()) {
			lState = *state.state;
			LoadImguiBindings();
		}
	}
}