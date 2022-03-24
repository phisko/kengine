#include "LogVisualStudioSystem.hpp"

// stl
#include <mutex>

// kengine
#include "kengine.hpp"
#include "data/AdjustableComponent.hpp"
#include "functions/Log.hpp"
#include "helpers/logHelper.hpp"

// putils
#include "magic_enum.hpp"
#include "thread_name.hpp"

namespace kengine {
	EntityCreator * LogVisualStudioSystem() noexcept {
		return [](Entity & e) noexcept {
#ifdef _WIN32
			auto & severityControl = e.attach<LogSeverityControl>();
            severityControl.severity = logHelper::parseCommandLineSeverity();

			e += AdjustableComponent{
				"Log", {
					{ "Visual Studio Console", &severityControl.severity }
				}
			};

			e += functions::Log{
				[&](const kengine::LogEvent & event) noexcept {
					if (event.severity < severityControl.severity)
						return;

					putils::string<4096> s;

					const auto & threadName = putils::get_thread_name();
					if (!threadName.empty())
						s += '{' + threadName + "}\t";
					s += putils::magic_enum::enum_name<LogSeverity>(event.severity);
					s += "\t[";
					s += event.category;
					s += "]\t";
					s += event.message;
					s += '\n';

					static std::mutex mutex;
					const std::lock_guard lock(mutex);
					OutputDebugStringA(s.str().c_str());
				}
			};
#endif
		};
	}
}
