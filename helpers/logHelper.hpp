#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "string.hpp"

// kengine functions
#include "functions/Log.hpp"

namespace kengine::logHelper {
    KENGINE_CORE_EXPORT void log(const entt::registry & r, LogSeverity severity, const char * category, const char * message) noexcept;
    KENGINE_CORE_EXPORT LogSeverity parseCommandLineSeverity(const entt::registry & r) noexcept;
}

#define kengine_log(registry, severity, category, message) kengine::logHelper::log(registry, kengine::LogSeverity::severity, category, message)
#define kengine_logf(registry, severity, category, format, ...) kengine_log(registry, severity, category, putils::string<1024>(format, __VA_ARGS__).c_str())
