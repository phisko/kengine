#include "log_stdout.hpp"

// stl
#include <iostream>
#include <mutex>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// termcolor
#include <termcolor/termcolor.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/thread_name.hpp"

// kengine core
#include "kengine/core/functions/log.hpp"
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine adjustable
#include "kengine/adjustable/data/adjustable.hpp"

namespace kengine::systems {
	struct log_stdout {
		std::mutex mutex;
		log_severity_control * severity_control = new log_severity_control; // Pointer to make sure it outlives log function component

		log_stdout(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*e.registry(), log, "log_stdout", "Initializing");

			*severity_control = log_helper::parse_command_line_severity(*e.registry());
			e.emplace<adjustable::adjustable>() = {
				"Log",
				{
					{ "Standard output", &severity_control->global_severity },
				}
			};

			e.emplace<functions::log>(putils_forward_to_this(log));
		}

		void log(const log_event & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!severity_control->passes(event))
				return;

			const std::lock_guard lock(mutex);

			if (event.severity == log_severity::warning)
				std::cout << termcolor::yellow;
			else if (event.severity == log_severity::error)
				std::cout << termcolor::red;

			const auto & thread_name = putils::get_thread_name();
			if (!thread_name.empty())
				std::cout << '{' << thread_name << "}\t";

			std::cout << magic_enum::enum_name<log_severity>(event.severity) << "\t[" << event.category << "]\t" << event.message << std::endl;
			std::cout << termcolor::reset;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(log_stdout)
}
