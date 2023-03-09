#include "create_entity.hpp"

// stl
#include <span>

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

	void create_entity(entt::registry & r, int argc, const char ** argv) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto e = r.create();
		const auto & comp = r.emplace<arguments>(e, putils::to_argument_vector(argc, argv));
		kengine_logf(r, log, log_category, "Creating command-line: {}", comp);
	}
}