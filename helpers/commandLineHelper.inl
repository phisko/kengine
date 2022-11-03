#include "commandLineHelper.hpp"
#include "kengine.hpp"

// putils
#include "command_line_arguments.hpp"

// kengine data
#include "data/CommandLineComponent.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine {
    template<typename T>
    T parseCommandLine() noexcept {
		KENGINE_PROFILING_SCOPE;

        for (const auto & [e, args] : entities.with<CommandLineComponent>())
            return putils::parseArguments<T>(args.arguments);
        return T{};
    }
}
