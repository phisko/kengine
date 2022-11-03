#include "logHelper.hpp"
#include "kengine.hpp"

// putils
#include "command_line_arguments.hpp"

// kengine data
#include "data/CommandLineComponent.hpp"

namespace {
    struct Options {
        kengine::LogSeverity logLevel = kengine::LogSeverity::Log;
    };
}

#define refltype Options
putils_reflection_info {
    putils_reflection_custom_class_name(Log);
    putils_reflection_attributes(
            putils_reflection_attribute(logLevel)
    );
};
#undef refltype

namespace kengine::logHelper {

    LogSeverity parseCommandLineSeverity() noexcept {
        static std::optional<LogSeverity> commandLineSeverity;
        if (commandLineSeverity != std::nullopt)
            return *commandLineSeverity;

        LogSeverity result = LogSeverity::Log;
        for (const auto & [e, commandLine] : entities.with<CommandLineComponent>()) {
            const auto options = putils::parseArguments<Options>(commandLine.arguments);
            result = options.logLevel;
        }
        commandLineSeverity = result;
        return *commandLineSeverity;
    }

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