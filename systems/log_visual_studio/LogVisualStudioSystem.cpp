#include "LogVisualStudioSystem.hpp"
#include "kengine.hpp"

// stl
#include <mutex>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "thread_name.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"

// kengine functions
#include "functions/Log.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"

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
					s += magic_enum::enum_name<LogSeverity>(event.severity);
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
