#include "command_line_helper.hpp"

// stl
#include <span>

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine command_line
#include "kengine/command_line/data/command_line.hpp"

namespace kengine {
	void create_command_line_entity(entt::registry & r, int argc, const char ** argv) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto e = r.create();
		auto & command_line = r.emplace<data::command_line>(e);
		std::stringstream s;
		for (const char * argument : std::span(argv, argc)) {
			command_line.arguments.emplace_back(argument);
			s << ' ' << argument;
		}

		kengine_logf(r, log, "command_line", "Creating command-line:%s", s.str().c_str());
	}
}