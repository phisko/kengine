#include "commandLineHelper.hpp"
#include "kengine.hpp"

// putils
#include "command_line_arguments.hpp"

// kengine data
#include "data/CommandLineComponent.hpp"

namespace kengine {
    template<typename T>
    T parseCommandLine() noexcept {
        for (const auto & [e, args] : entities.with<CommandLineComponent>())
            return putils::parseArguments<T>(args.arguments);
        return T{};
    }
}
