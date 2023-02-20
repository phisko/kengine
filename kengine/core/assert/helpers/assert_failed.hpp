#pragma once

// entt
#include <entt/entity/registry.hpp>

namespace kengine::core::assert {
	KENGINE_CORE_ASSERT_EXPORT bool assert_failed(const entt::registry & r, const char * file, int line, const std::string & expr) noexcept;
}