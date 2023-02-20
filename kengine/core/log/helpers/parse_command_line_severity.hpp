#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine core/log
#include "kengine/core/log/data/severity_control.hpp"
#include "kengine/core/log/functions/on_log.hpp"

namespace kengine::core::log {
	KENGINE_CORE_LOG_EXPORT severity_control parse_command_line_severity(const entt::registry & r) noexcept;
}
