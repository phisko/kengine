#include "system.hpp"

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

// kengine
#include "kengine/adjustable/data/adjustable.hpp"
#include "kengine/core/log/data/severity_control.hpp"
#include "kengine/core/log/functions/on_log.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/log/helpers/parse_command_line_severity.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::core::log::standard_output {
	static constexpr auto log_category = "core_log_standard_output";

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
					{ "Standard output", &control.global_severity },
				}
			};
		}

		void log(const event & log_event) noexcept {
			KENGINE_PROFILING_SCOPE;

			const std::lock_guard lock(mutex);

			if (log_event.message_severity == severity::warning)
				std::cout << termcolor::yellow;
			else if (log_event.message_severity == severity::error)
				std::cout << termcolor::red;

			const auto & thread_name = putils::get_thread_name();
			if (!thread_name.empty())
				std::cout << '{' << thread_name << "}\t";

			std::cout << magic_enum::enum_name<severity>(log_event.message_severity) << "\t[" << log_event.category << "]\t" << log_event.message << std::endl;
			std::cout << termcolor::reset;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
