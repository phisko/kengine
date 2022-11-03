#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine {
    enum class LogSeverity {
        All,
        Verbose,
        Log,
        Warning,
        Error,
        None
    };

    struct LogEvent {
        LogSeverity severity;
        const char * category;
        const char * message;
    };

    struct LogSeverityControl {
        LogSeverity severity = LogSeverity::Log;
    };

    namespace functions {
        struct Log : BaseFunction<
            void(const LogEvent & log)
        > {};
    }
}