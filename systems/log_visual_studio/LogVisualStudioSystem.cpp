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
#include "helpers/profilingHelper.hpp"

namespace kengine {
	namespace {
		struct logVisualStudioImpl {
			static inline const LogSeverity * severity = nullptr;

			static void init(Entity & e) noexcept {
#ifdef _WIN32
				KENGINE_PROFILING_SCOPE;

				auto & severityControl = e.attach<LogSeverityControl>();
				severityControl.severity = logHelper::parseCommandLineSeverity();
				severity = &severityControl.severity;

				e += AdjustableComponent{
					"Log", {
						{ "Visual Studio Console", &severityControl.severity }
					}
				};

				e += functions::Log{ log };
#endif
			}

#ifdef _WIN32
			static void log(const LogEvent & event) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (event.severity < *severity)
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
#endif
		};
	}

	EntityCreator * LogVisualStudioSystem() noexcept {
		KENGINE_PROFILING_SCOPE;
		return logVisualStudioImpl::init;
	}
}
