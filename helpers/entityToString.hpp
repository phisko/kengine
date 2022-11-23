#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "to_string.hpp"

namespace putils {
	template<>
	KENGINE_CORE_EXPORT std::string toString(const entt::entity & e) noexcept;

	template<>
	KENGINE_CORE_EXPORT void parse(entt::entity & e, std::string_view str) noexcept;
}