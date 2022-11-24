#include "LogFileSystem.hpp"

// stl
#include <fstream>
#include <mutex>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "forward_to.hpp"
#include "thread_name.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"

// kengine functions
#include "functions/Log.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/commandLineHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

#ifndef KENGINE_LOG_FILE_LOCATION
# define KENGINE_LOG_FILE_LOCATION "kengine.log"
#endif

namespace kengine {
	struct LogFileSystem {
		std::ofstream file;
		std::mutex mutex;
		const LogSeverity * severity = nullptr;

		LogFileSystem(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & r = *e.registry();

			const auto options = kengine::parseCommandLine<Options>(r);
			const char * fileName = options.logFile ? options.logFile->c_str() : KENGINE_LOG_FILE_LOCATION;

			file.open(fileName);
			if (!file) {
				kengine_assert_failed(r, "LogFileSystem couldn't open output file '%s'", fileName);
				return;
			}

			auto & severityControl = e.emplace<LogSeverityControl>();
			severityControl.severity = logHelper::parseCommandLineSeverity(r);
			severity = &severityControl.severity;

			e.emplace<AdjustableComponent>() = {
				"Log", {
					{ "File", severity }
				}
			};

			e.emplace<functions::Log>(putils_forward_to_this(log));
		}

		void log(const LogEvent & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (event.severity < *severity)
				return;

			const std::lock_guard lock(mutex);

			const auto & threadName = putils::get_thread_name();
			if (!threadName.empty())
				file << '{' << threadName << "}\t";

			file << magic_enum::enum_name<LogSeverity>(event.severity) << "\t[" << event.category << "]\t"
				 << event.message << std::endl;
		}

		// Command-line arguments
		struct Options {
			std::optional<std::string> logFile;
		};
	};

	void addLogFileSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<LogFileSystem>(e);
	}
}

#define refltype kengine::LogFileSystem::Options
putils_reflection_info {
	putils_reflection_custom_class_name(Log file)
	putils_reflection_attributes(
		putils_reflection_attribute(logFile)
	)
};
#undef refltype