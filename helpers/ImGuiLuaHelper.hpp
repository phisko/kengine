#pragma once

namespace kengine { class EntityManager; }

namespace kengine::ImGuiLuaHelper {
	void initBindings(EntityManager & em);
}