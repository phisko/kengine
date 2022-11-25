#include "command_line_helper.hpp"

// stl
#include <span>

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/command_line.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	void create_command_line_entity(entt::registry & r, int argc, const char ** argv) noexcept {
		KENGINE_PROFILING_SCOPE;
		const auto e = r.create();
		auto & command_line = r.emplace<data::command_line>(e);
		for (const char * argument : std::span(argv, argc))
			command_line.arguments.emplace_back(argument);
	}
}