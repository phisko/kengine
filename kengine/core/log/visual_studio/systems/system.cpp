#include "system.hpp"

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

// kengine adjustable
#include "kengine/adjustable/data/adjustable.hpp"

// kengine core/log
#include "kengine/core/log/data/severity_control.hpp"
#include "kengine/core/log/functions/on_log.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/log/helpers/parse_command_line_severity.hpp"

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::core::log::visual_studio {
	static constexpr auto log_category = "core_log_visual_studio";

	struct system {
		std::mutex mutex;

		system(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			e.emplace<on_log>(putils_forward_to_this(log));

			kengine_log(*e.registry(), log, log_category, "Initializing");

			auto & control = e.emplace<severity_control>(parse_command_line_severity(*e.registry()));
			e.emplace<adjustable::adjustable>() = {
				"Log",
				{
					{ "Visual Studio Console", &control.global_severity },
				}
			};
		}

		void log(const event & log_event) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::string<4096> s;

			const auto & thread_name = putils::get_thread_name();
			if (!thread_name.empty())
				s += '{' + thread_name + "}\t";
			s += magic_enum::enum_name<severity>(log_event.message_severity);
			s += "\t[";
			s += log_event.category;
			s += "]\t";
			s += log_event.message;
			s += '\n';

			const std::lock_guard lock(mutex);
			OutputDebugStringA(s.str().c_str());
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}

#endif