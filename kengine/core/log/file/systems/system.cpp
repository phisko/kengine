#include "system.hpp"

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

// kengine adjustable
#include "kengine/adjustable/data/adjustable.hpp"

// kengine core/assert
#include "kengine/core/assert/helpers/kengine_assert.hpp"

// kengine core/log
#include "kengine/core/log/data/severity_control.hpp"
#include "kengine/core/log/functions/on_log.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/log/helpers/parse_command_line_severity.hpp"

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine command_line
#include "kengine/command_line/helpers/parse.hpp"

#ifndef KENGINE_LOG_FILE_LOCATION
#define KENGINE_LOG_FILE_LOCATION "kengine.log"
#endif

namespace kengine::core::log::file {
	static constexpr auto log_category = "core_log_file";

	struct system {
		std::ofstream file;
		std::mutex mutex;

		system(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			e.emplace<on_log>(putils_forward_to_this(log));

			kengine_log(*e.registry(), log, log_category, "Initializing");

			const auto & r = *e.registry();

			const auto args = kengine::command_line::parse<options>(r);
			const char * file_name = args.log_file ? args.log_file->c_str() : KENGINE_LOG_FILE_LOCATION;

			file.open(file_name);
			if (!file) {
				kengine_assert_failed(r, "log_file system couldn't open output file '%s'", file_name);
				return;
			}

			auto & control = e.emplace<severity_control>(parse_command_line_severity(r));
			e.emplace<adjustable::adjustable>() = {
				"Log",
				{
					{ "File", &control.global_severity },
				}
			};
		}

		void log(const event & log_event) noexcept {
			KENGINE_PROFILING_SCOPE;

			const std::lock_guard lock(mutex);

			const auto & thread_name = putils::get_thread_name();
			if (!thread_name.empty())
				file << '{' << thread_name << "}\t";

			file << magic_enum::enum_name<severity>(log_event.message_severity) << "\t[" << log_event.category << "]\t"
				 << log_event.message << std::endl;
		}

		// Command-line arguments
		struct options {
			std::optional<std::string> log_file;
		};
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}

#define refltype kengine::core::log::file::system::options
putils_reflection_info {
	putils_reflection_custom_class_name(log file);
	putils_reflection_attributes(
		putils_reflection_attribute(log_file)
	)
};
#undef refltype