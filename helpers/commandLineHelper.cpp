#include "commandLineHelper.hpp"

// stl
#include <span>

// kengine data
#include "data/CommandLineComponent.hpp"

namespace kengine {
	void createCommandLineEntity(int argc, const char ** argv) noexcept {
		entities += [&](Entity & e) noexcept {
			auto & commandLine = e.attach<CommandLineComponent>();
			for (const char * argument : std::span(argv, argc))
				commandLine.arguments.emplace_back(argument);
		};
	}
}