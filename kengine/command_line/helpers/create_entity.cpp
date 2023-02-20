#include "create_entity.hpp"

// stl
#include <span>
#include <sstream>

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine command_line
#include "kengine/command_line/data/arguments.hpp"

namespace kengine::command_line {
	void create_entity(entt::registry & r, int argc, const char ** argv) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto e = r.create();
		auto & comp = r.emplace<arguments>(e);
		std::stringstream s;
		for (const char * argument : std::span(argv, argc)) {
			comp.args.emplace_back(argument);
			s << ' ' << argument;
		}

		kengine_logf(r, log, "command_line", "Creating command-line:%s", s.str().c_str());
	}
}