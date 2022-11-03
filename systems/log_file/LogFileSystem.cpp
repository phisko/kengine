#include "LogFileSystem.hpp"
#include "kengine.hpp"

// stl
#include <fstream>
#include <mutex>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "thread_name.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"

// kengine functions
#include "functions/Log.hpp"

// kengine helpers
#include "helpers/commandLineHelper.hpp"
#include "helpers/logHelper.hpp"

#ifndef KENGINE_LOG_FILE_LOCATION
# define KENGINE_LOG_FILE_LOCATION "kengine.log"
#endif

namespace {
    // Command-line arguments
    struct Options {
        std::optional<std::string> logFile;
    };
}

#define refltype Options
putils_reflection_info {
    putils_reflection_custom_class_name(Log file)
    putils_reflection_attributes(
        putils_reflection_attribute(logFile)
    )
};
#undef refltype

namespace kengine {
	EntityCreator * LogFileSystem() noexcept {
		return [](Entity & e) noexcept {
            const auto options = kengine::parseCommandLine<Options>();

            const char * fileName = options.logFile ?
                    options.logFile->c_str() :
                    KENGINE_LOG_FILE_LOCATION;

            static std::ofstream file(fileName);

            if (!file) {
                kengine_assert_failed("LogFileSystem couldn't open output file '%s'", fileName);
                return;
            }

			auto & severityControl = e.attach<LogSeverityControl>();
            severityControl.severity = logHelper::parseCommandLineSeverity();

			e += AdjustableComponent{
				"Log", {
					{ "File", &severityControl.severity }
				}
			};

			e += functions::Log{
				[&](const kengine::LogEvent & event) noexcept {
					static std::mutex mutex;
					if (event.severity < severityControl.severity)
						return;

					const std::lock_guard lock(mutex);

					const auto & threadName = putils::get_thread_name();
					if (!threadName.empty())
						file << '{' << threadName << "}\t";

					file << magic_enum::enum_name<LogSeverity>(event.severity) << "\t[" << event.category << "]\t" << event.message << std::endl;
				}
			};
		};
	}
}
