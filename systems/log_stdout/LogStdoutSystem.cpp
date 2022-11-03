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
		};
	}
}
