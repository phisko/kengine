#pragma once

#include "functions/Log.hpp"
#include "string.hpp"

namespace kengine::logHelper {
    void log(LogSeverity severity, const char * category, const char * message) noexcept;
}

#define kengine_log(severity, category, message) kengine::logHelper::log(kengine::LogSeverity::severity, category, message)
#define kengine_logf(severity, category, format, ...) kengine_log(severity, category, putils::string<1024>(format, __VA_ARGS__))
