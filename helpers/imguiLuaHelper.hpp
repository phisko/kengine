#pragma once

#ifdef KENGINE_LUA

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::imguiLuaHelper {
	KENGINE_CORE_EXPORT void initBindings(const entt::registry & r) noexcept;
}

#endif