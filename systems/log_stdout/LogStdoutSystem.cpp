#include "LogStdoutSystem.hpp"
#include "kengine.hpp"

// stl
#include <iostream>
#include <mutex>

// magic_enum
#include <magic_enum.hpp>

// termcolor
#include <termcolor/termcolor.hpp>

// putils
#include "thread_name.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"

// kengine functions
#include "functions/Log.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	namespace {
		struct logStdoutImpl {
			static inline const LogSeverity * severity = nullptr;

			static void init(Entity & e) noexcept {
				KENGINE_PROFILING_SCOPE;

				auto & severityControl = e.attach<LogSeverityControl>();
				severityControl.severity = logHelper::parseCommandLineSeverity();
				severity = &severityControl.severity;

				e += AdjustableComponent{
					"Log", {
						{ "Standard output", &severityControl.severity }
					}
				};

				e += functions::Log{ log };
			}

			static void log(const LogEvent & event) noexcept {
				KENGINE_PROFILING_SCOPE;

				static std::mutex mutex;
				if (event.severity < *severity)
					return;

				const std::lock_guard lock(mutex);

				if (event.severity == LogSeverity::Warning)
					std::cout << termcolor::yellow;
				else if (event.severity == LogSeverity::Error)
					std::cout << termcolor::red;

				const auto & threadName = putils::get_thread_name();
				if (!threadName.empty())
					std::cout << '{' << threadName << "}\t";

				std::cout << magic_enum::enum_name<LogSeverity>(event.severity) << "\t[" << event.category << "]\t" << event.message << std::endl;
				std::cout << termcolor::reset;
			}
		};
	}

	EntityCreator * LogStdoutSystem() noexcept {
		KENGINE_PROFILING_SCOPE;
		return logStdoutImpl::init;
	}
}
