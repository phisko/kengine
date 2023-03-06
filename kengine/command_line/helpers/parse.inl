#include "parse.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/command_line_arguments.hpp"

// kengine
#include "kengine/command_line/data/arguments.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::command_line {
	static constexpr auto log_category = "command_line";

	template<typename T>
	T parse(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, log_category, "Parsing command-line");

		for (const auto & [e, args] : r.view<arguments>().each()) {
			kengine_logf(r, very_verbose, log_category, "Parsing command-line from {}", e);
			return putils::parse_arguments<T>(args.args);
		}

		kengine_log(r, verbose, log_category, "Found no command-line entity");
		return T{};
	}
}
