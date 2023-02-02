#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/string.hpp"

// kengine functions
#include "kengine/functions/log.hpp"

namespace kengine::log_helper {
	KENGINE_CORE_EXPORT void log(const entt::registry & r, log_severity severity, const char * category, const char * message) noexcept;
	KENGINE_CORE_EXPORT log_severity parse_command_line_severity(const entt::registry & r) noexcept;
}

#ifdef KENGINE_NO_LOG
#define kengine_log(...) (void)0
#define kengine_logf(...) (void)0
#else
#define kengine_log(registry, severity, category, message) kengine::log_helper::log(registry, kengine::log_severity::severity, category, message)
#define kengine_logf(registry, severity, category, format, ...) kengine_log(registry, severity, category, putils::string<1024>(format, __VA_ARGS__).c_str())
#endif
