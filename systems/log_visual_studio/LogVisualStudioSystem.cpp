#include "LogVisualStudioSystem.hpp"

#ifdef _WIN32

// stl
#include <mutex>

// windows
#include <windows.h>
#include <debugapi.h>

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
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct LogVisualStudioSystem {
		static void init(entt::registry & r) noexcept {
#ifdef _WIN32
			KENGINE_PROFILING_SCOPE;

			const auto e = r.create();
			auto & severityControl = r.emplace<LogSeverityControl>(e);
			severityControl.severity = logHelper::parseCommandLineSeverity(r);
			_severity = &severityControl.severity;

			r.emplace<AdjustableComponent>(e) = {
				"Log", {
					{ "Visual Studio Console", _severity }
				}
			};

			r.emplace<functions::Log>(e, log);
#endif
		}

#ifdef _WIN32
		static void log(const LogEvent & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (event.severity < *_severity)
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

		static inline const LogSeverity * _severity = nullptr;
	};

	void LogVisualStudioSystem(entt::registry & r) noexcept {
		LogVisualStudioSystem::init(r);
	}
}

#endif