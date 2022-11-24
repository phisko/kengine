#include "LogVisualStudioSystem.hpp"

#ifdef _WIN32

// stl
#include <mutex>

// windows
#include <windows.h>
#include <debugapi.h>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

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
	struct LogVisualStudioSystem {
		const LogSeverity * severity = nullptr;
		std::mutex mutex;

		LogVisualStudioSystem(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & severityControl = e.emplace<LogSeverityControl>();
			severityControl.severity = logHelper::parseCommandLineSeverity(*e.registry());
			severity = &severityControl.severity;

			e.emplace<AdjustableComponent>() = {
				"Log", {
					{ "Visual Studio Console", severity }
				}
			};

			e.emplace<functions::Log>(putils_forward_to_this(log));
		}

		void log(const LogEvent & event) noexcept {
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

			const std::lock_guard lock(mutex);
			OutputDebugStringA(s.str().c_str());
		}
	};

	void addLogVisualStudioSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<LogVisualStudioSystem>(e);
	}
}

#endif