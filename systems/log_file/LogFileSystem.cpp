#include "LogFileSystem.hpp"

// stl
#include <fstream>
#include <mutex>

// entt
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// putils
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
		static void init(entt::registry & r) noexcept {
			const auto options = kengine::parseCommandLine<Options>(r);
			const char * fileName = options.logFile ? options.logFile->c_str() : KENGINE_LOG_FILE_LOCATION;

			_file.open(fileName);
			if (!_file) {
				kengine_assert_failed(r, "LogFileSystem couldn't open output file '%s'", fileName);
				return;
			}

			const auto e = r.create();

			auto & severityControl = r.emplace<LogSeverityControl>(e);
			severityControl.severity = logHelper::parseCommandLineSeverity(r);
			_severity = &severityControl.severity;

			r.emplace<AdjustableComponent>(e) = {
				"Log", {
					{ "File", _severity }
				}
			};

			r.emplace<functions::Log>(e, log);
		}

		static void log(const LogEvent & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			static std::mutex mutex;
			if (event.severity < *_severity)
				return;

			const std::lock_guard lock(mutex);

			const auto & threadName = putils::get_thread_name();
			if (!threadName.empty())
				_file << '{' << threadName << "}\t";

			_file << magic_enum::enum_name<LogSeverity>(event.severity) << "\t[" << event.category << "]\t"
				 << event.message << std::endl;
		}

		static inline std::ofstream _file;
		static inline const LogSeverity * _severity = nullptr;

		// Command-line arguments
		struct Options {
			std::optional<std::string> logFile;
		};
	};

	void LogFileSystem(entt::registry & r) noexcept {
		LogFileSystem::init(r);
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