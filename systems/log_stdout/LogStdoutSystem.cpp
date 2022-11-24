#include "LogStdoutSystem.hpp"

// stl
#include <iostream>
#include <mutex>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// termcolor
#include <termcolor/termcolor.hpp>

// putils
#include "forward_to.hpp"
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
		std::mutex mutex;
		LogSeverity * severity = nullptr;

		LogStdoutSystem(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & severityControl = e.emplace<LogSeverityControl>();
			severityControl.severity = logHelper::parseCommandLineSeverity(*e.registry());
			severity = &severityControl.severity;

			e.emplace<AdjustableComponent>() = {
				"Log", {
					{ "Standard output", severity }
				}
			};

			e.emplace<functions::Log>(putils_forward_to_this(log));
		}

		void log(const LogEvent & event) noexcept {
			KENGINE_PROFILING_SCOPE;

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

	void addLogStdoutSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<LogStdoutSystem>(e);
	}
}
