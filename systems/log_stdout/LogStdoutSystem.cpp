#include "LogStdoutSystem.hpp"

// stl
#include <iostream>
#include <mutex>

// kengine
#include "kengine.hpp"
#include "data/AdjustableComponent.hpp"
#include "functions/Log.hpp"
#include "helpers/logHelper.hpp"

// putils
#include "magic_enum.hpp"
#include "termcolor.hpp"
#include "thread_name.hpp"

namespace kengine {
	EntityCreator * LogStdoutSystem() noexcept {
		return [](Entity & e) noexcept {
			auto & severityControl = e.attach<LogSeverityControl>();
            severityControl.severity = logHelper::parseCommandLineSeverity();

			e += AdjustableComponent{
				"Log", {
					{ "Standard output", &severityControl.severity }
				}
			};

			e += functions::Log{
				[&](const kengine::LogEvent & event) noexcept {
					static std::mutex mutex;
					if (event.severity < severityControl.severity)
						return;

					const std::lock_guard lock(mutex);

					if (event.severity == LogSeverity::Warning)
						std::cout << putils::termcolor::yellow;
					else if (event.severity == LogSeverity::Error)
						std::cout << putils::termcolor::red;

					const auto & threadName = putils::get_thread_name();
					if (!threadName.empty())
						std::cout << '{' << threadName << "}\t";

					std::cout << putils::magic_enum::enum_name<LogSeverity>(event.severity) << "\t[" << event.category << "]\t" << event.message << '\n';

					std::cout << putils::termcolor::reset;
				}
			};
		};
	}
}
