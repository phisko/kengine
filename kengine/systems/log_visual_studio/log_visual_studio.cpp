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
		log_severity_control severity_control;
		std::mutex mutex;

		log_visual_studio(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*e.registry(), log, "log_visual_studio", "Initializing");

			severity_control = log_helper::parse_command_line_severity(*e.registry());
			e.emplace<data::adjustable>() = {
				"Log",
				{
					{ "Visual Studio Console", &severity_control.global_severity },
				}
			};

			e.emplace<functions::log>(putils_forward_to_this(log));
		}

		void log(const log_event & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!severity_control.passes(event))
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

	DEFINE_KENGINE_SYSTEM_CREATOR(log_visual_studio)
}

#endif