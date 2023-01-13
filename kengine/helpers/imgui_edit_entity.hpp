#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

namespace putils::reflection {
	// Overload these for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	KENGINE_CORE_EXPORT void imgui_edit(const char * name, const entt::entity & obj) noexcept;

	template<>
	KENGINE_CORE_EXPORT void imgui_edit(const char * name, entt::entity & obj) noexcept;
}