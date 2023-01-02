#include "log_visual_studio.hpp"

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
#include "putils/forward_to.hpp"
#include "putils/thread_name.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"

// kengine functions
#include "kengine/functions/log.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct log_visual_studio {
		const log_severity * severity = nullptr;
		std::mutex mutex;

		log_visual_studio(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & severity_control = e.emplace<log_severity_control>();
			severity_control.severity = log_helper::parse_command_line_severity(*e.registry());
			severity = &severity_control.severity;

			e.emplace<data::adjustable>() = {
				"log",
				{
					{ "Visual Studio Console", severity },
				}
			};

			e.emplace<functions::log>(putils_forward_to_this(log));
		}

		void log(const log_event & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (event.severity < *severity)
				return;

			putils::string<4096> s;

			const auto & thread_name = putils::get_thread_name();
			if (!thread_name.empty())
				s += '{' + thread_name + "}\t";
			s += magic_enum::enum_name<log_severity>(event.severity);
			s += "\t[";
			s += event.category;
			s += "]\t";
			s += event.message;
			s += '\n';

			const std::lock_guard lock(mutex);
			OutputDebugStringA(s.str().c_str());
		}
	};

	void add_log_visual_studio(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<log_visual_studio>(e);
	}
}

#endif