#include "LogStdoutSystem.hpp"

// stl
#include <iostream>
#include <mutex>

// entt
#include <entt/entity/registry.hpp>

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
	struct LogStdoutSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto e = r.create();
			auto & severityControl = r.emplace<LogSeverityControl>(e);
			severityControl.severity = logHelper::parseCommandLineSeverity(r);
			_severity = &severityControl.severity;

			r.emplace<AdjustableComponent>(e) = {
				"Log", {
					{ "Standard output", _severity }
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

		static inline const LogSeverity * _severity = nullptr;
	};

	void LogStdoutSystem(entt::registry & r) noexcept {
		LogStdoutSystem::init(r);
	}
}
