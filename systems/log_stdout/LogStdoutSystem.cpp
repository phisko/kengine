#include "LogStdoutSystem.hpp"

#include <iostream>
#include <iomanip>
#include <mutex>

#include "kengine.hpp"
#include "data/AdjustableComponent.hpp"
#include "functions/Log.hpp"
#include "magic_enum.hpp"
#include "termcolor.hpp"
#include "thread_name.hpp"

namespace kengine {
	EntityCreator * LogStdoutSystem() noexcept {
		return [](Entity & e) noexcept {
			auto & severityControl = e.attach<LogSeverityControl>();

			e += AdjustableComponent{
				"Log", {
					{ "Standard output", &severityControl.severity }
				}
			};

			e += functions::Log{
				[&](LogSeverity severity, const char * category, const char * message) noexcept {
					static std::mutex mutex;
					if (severity < severityControl.severity)
						return;

					const std::lock_guard lock(mutex);

					if (severity == LogSeverity::Warning)
						std::cout << putils::termcolor::yellow;
					else if (severity == LogSeverity::Error)
						std::cout << putils::termcolor::red;

					const auto & threadName = putils::get_thread_name();
					if (!threadName.empty())
						std::cout << '{' << threadName << "}\t";

					std::cout << putils::magic_enum::enum_name<LogSeverity>(severity) << "\t[" << category << "]\t" << message << '\n';

					std::cout << putils::termcolor::reset;
				}
			};
		};
	}
}
