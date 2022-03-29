#include "commandLineHelper.hpp"
#include "kengine.hpp"

#include "data/CommandLineComponent.hpp"
#include "command_line_arguments.hpp"

namespace kengine {
    template<typename T>
    T parseCommandLine() noexcept {
        for (const auto & [e, args] : entities.with<CommandLineComponent>())
            return putils::parseArguments<T>(args.arguments);
        return T{};
    }
}
