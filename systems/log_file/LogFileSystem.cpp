#include "LogFileSystem.hpp"

// stl
#include <fstream>
#include <mutex>

// kengine
#include "kengine.hpp"
#include "data/AdjustableComponent.hpp"
#include "functions/Log.hpp"
#include "helpers/logHelper.hpp"

// putils
#include <magic_enum.hpp>
#include "thread_name.hpp"

#ifndef KENGINE_LOG_FILE_LOCATION
# define KENGINE_LOG_FILE_LOCATION "kengine.log"
#endif

namespace kengine {
    namespace {
        std::ofstream file(KENGINE_LOG_FILE_LOCATION);
    }

	EntityCreator * LogFileSystem() noexcept {
		return [](Entity & e) noexcept {
            if (!file) {
                kengine_assert_failed("LogFileSystem couldn't open output file '%s'", KENGINE_LOG_FILE_LOCATION);
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
