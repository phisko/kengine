#include "parse.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/command_line_arguments.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine command_line
#include "kengine/command_line/data/arguments.hpp"

namespace kengine::command_line {
	template<typename T>
	T parse(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "command_line", "Parsing command-line");

		for (const auto & [e, args] : r.view<arguments>().each()) {
			kengine_logf(r, very_verbose, "command_line", "Parsing command-line from [%u]", e);
			return putils::parse_arguments<T>(args.args);
		}

		kengine_log(r, verbose, "command_line", "Found no command-line entity");
		return T{};
	}
}
