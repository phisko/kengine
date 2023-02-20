#pragma once

// entt
#include <entt/entity/registry.hpp>

// kengine core/log
#include "kengine/core/log/functions/on_log.hpp"

namespace kengine::core::log {
	KENGINE_CORE_LOG_EXPORT void log(const entt::registry & r, severity severity, const char * category, const char * message) noexcept;
}