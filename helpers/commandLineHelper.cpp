#include "commandLineHelper.hpp"

// stl
#include <span>

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "data/CommandLineComponent.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine {
	void createCommandLineEntity(entt::registry & r, int argc, const char ** argv) noexcept {
		KENGINE_PROFILING_SCOPE;
		const auto e = r.create();
		auto & commandLine = r.emplace<CommandLineComponent>(e);
		for (const char * argument : std::span(argv, argc))
			commandLine.arguments.emplace_back(argument);
	}
}