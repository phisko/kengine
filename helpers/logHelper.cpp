#include "logHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "command_line_arguments.hpp"

// kengine data
#include "data/CommandLineComponent.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

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
    LogSeverity parseCommandLineSeverity(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

        static std::optional<LogSeverity> commandLineSeverity;
        if (commandLineSeverity != std::nullopt)
            return *commandLineSeverity;

        LogSeverity result = LogSeverity::Log;
        for (const auto & [e, commandLine] : r.view<CommandLineComponent>().each()) {
            const auto options = putils::parseArguments<Options>(commandLine.arguments);
            result = options.logLevel;
        }
        commandLineSeverity = result;
        return *commandLineSeverity;
    }

    void log(const entt::registry & r, LogSeverity severity, const char * category, const char * message) noexcept {
		KENGINE_PROFILING_SCOPE;

        const kengine::LogEvent event{
            .severity = severity,
            .category = category,
            .message = message
        };

        for (const auto & [e, log] : r.view<functions::Log>().each())
            log(event);
    }
}