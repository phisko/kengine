#include "logHelper.hpp"
#include "kengine.hpp"

namespace kengine::logHelper {
    void log(LogSeverity severity, const char * category, const char * message) noexcept {
        const kengine::LogEvent event{
            .severity = severity,
            .category = category,
            .message = message
        };

        for (const auto & [e, log] : entities.with<functions::Log>())
            log(event);
    }
}