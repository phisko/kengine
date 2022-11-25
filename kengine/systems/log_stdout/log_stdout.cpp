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

// kengine data
#include "kengine/data/adjustable.hpp"

// kengine functions
#include "kengine/functions/log.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct log_stdout {
		std::mutex mutex;
		log_severity * severity = nullptr;

		log_stdout(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & severity_control = e.emplace<log_severity_control>();
			severity_control.severity = log_helper::parse_command_line_severity(*e.registry());
			severity = &severity_control.severity;

			e.emplace<data::adjustable>() = {
				"log", {
					{ "Standard output", severity }
				}
			};

			e.emplace<functions::log>(putils_forward_to_this(log));
		}

		void log(const log_event & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (event.severity < *severity)
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

	void add_log_stdout(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<log_stdout>(e);
	}
}
