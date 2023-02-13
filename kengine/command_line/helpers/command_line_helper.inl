#include "command_line_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/command_line_arguments.hpp"

// kengine core
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine command_line
#include "kengine/command_line/data/command_line.hpp"

namespace kengine {
	template<typename T>
	T parse_command_line(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "command_line", "Parsing command-line");

		for (const auto & [e, args] : r.view<data::command_line>().each()) {
			kengine_logf(r, very_verbose, "command_line", "Parsing command-line from [%u]", e);
			return putils::parse_arguments<T>(args.arguments);
		}

		kengine_log(r, verbose, "command_line", "Found no command-line entity");
		return T{};
	}
}
