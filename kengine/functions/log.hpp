#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine {
    enum class log_severity {
        all,
        verbose,
        log,
        warning,
        error,
        none
    };

    struct log_event {
        log_severity severity;
        const char * category;
        const char * message;
    };

    struct log_severity_control {
        log_severity severity = log_severity::log;
    };

    namespace functions {
        struct log : base_function<
            void(const log_event & log)
        > {};
    }
}