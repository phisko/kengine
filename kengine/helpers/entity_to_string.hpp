#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/to_string.hpp"

namespace putils {
	KENGINE_CORE_EXPORT std::string to_string(const entt::entity & e) noexcept;

	template<>
	KENGINE_CORE_EXPORT void parse(entt::entity & e, std::string_view str) noexcept;
}