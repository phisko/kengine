#pragma once

#include "BaseFunction.hpp"

namespace kengine {
    enum class LogSeverity {
        Verbose,
        Log,
        Warning,
        Error
    };

    struct LogSeverityControl {
        LogSeverity severity = LogSeverity::Log;
    };

    namespace functions {
        struct Log : BaseFunction<
            void(LogSeverity severity, const char * category, const char * message)
        > {};
    }
}