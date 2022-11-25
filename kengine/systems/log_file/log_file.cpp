#include "log_file.hpp"

// stl
#include <fstream>
#include <mutex>

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
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/command_line_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

#ifndef KENGINE_LOG_FILE_LOCATION
# define KENGINE_LOG_FILE_LOCATION "kengine.log"
#endif

namespace kengine::systems {
	struct log_file {
		std::ofstream file;
		std::mutex mutex;
		const log_severity * severity = nullptr;

		log_file(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & r = *e.registry();

			const auto args = kengine::parse_command_line<options>(r);
			const char * file_name = args.log_file ? args.log_file->c_str() : KENGINE_LOG_FILE_LOCATION;

			file.open(file_name);
			if (!file) {
				kengine_assert_failed(r, "log_file system couldn't open output file '%s'", file_name);
				return;
			}

			auto & severity_control = e.emplace<log_severity_control>();
			severity_control.severity = log_helper::parse_command_line_severity(r);
			severity = &severity_control.severity;

			e.emplace<data::adjustable>() = {
				"log", {
					{ "File", severity }
				}
			};

			e.emplace<functions::log>(putils_forward_to_this(log));
		}

		void log(const log_event & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (event.severity < *severity)
				return;

			const std::lock_guard lock(mutex);

			const auto & thread_name = putils::get_thread_name();
			if (!thread_name.empty())
				file << '{' << thread_name << "}\t";

			file << magic_enum::enum_name<log_severity>(event.severity) << "\t[" << event.category << "]\t"
				 << event.message << std::endl;
		}

		// Command-line arguments
		struct options {
			std::optional<std::string> log_file;
		};
	};

	void add_log_file(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<log_file>(e);
	}
}

#define refltype kengine::systems::log_file::options
putils_reflection_info {
	putils_reflection_custom_class_name(log file)
	putils_reflection_attributes(
		putils_reflection_attribute(log_file)
	)
};
#undef refltype