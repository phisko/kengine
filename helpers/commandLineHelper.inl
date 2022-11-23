#include "commandLineHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "command_line_arguments.hpp"

// kengine data
#include "data/CommandLineComponent.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine {
    template<typename T>
    T parseCommandLine(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

        for (const auto & [e, args] : r.view<CommandLineComponent>().each())
            return putils::parseArguments<T>(args.arguments);
        return T{};
    }
}
