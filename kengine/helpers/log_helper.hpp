#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/string.hpp"

// kengine functions
#include "kengine/functions/log.hpp"

namespace kengine::log_helper {
	KENGINE_CORE_EXPORT void log(const entt::registry & r, log_severity severity, const char * category, const char * message) noexcept;
	KENGINE_CORE_EXPORT log_severity_control parse_command_line_severity(const entt::registry & r) noexcept;
	KENGINE_CORE_EXPORT void set_minimum_log_severity(const entt::registry & r, log_severity severity) noexcept;
}

#ifndef KENGINE_LOG_MAX_SEVERITY
#define KENGINE_LOG_MAX_SEVERITY all
#endif

#ifdef KENGINE_NO_LOG
#define kengine_log(...) (void)0
#define kengine_logf(...) (void)0
#else
#define kengine_log(registry, severity, category, message)\
	do {\
		if constexpr (kengine::log_severity::severity >= kengine::log_severity::KENGINE_LOG_MAX_SEVERITY)\
			kengine::log_helper::log(registry, kengine::log_severity::severity, category, message);\
	} while (false)
#define kengine_logf(registry, severity, category, format, ...) kengine_log(registry, severity, category, putils::string<1024>(format, __VA_ARGS__).c_str())
#endif
