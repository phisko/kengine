#include "logHelper.hpp"
#include "kengine.hpp"

namespace kengine::logHelper {
    void log(LogSeverity severity, const char * category, const char * message) noexcept {
        for (const auto & [e, log] : entities.with<functions::Log>())
            log(severity, category, message);
    }
}