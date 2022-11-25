#include "command_line_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/command_line_arguments.hpp"

// kengine data
#include "kengine/data/command_line.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
    template<typename T>
    T parse_command_line(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

        for (const auto & [e, args] : r.view<data::command_line>().each())
            return putils::parse_arguments<T>(args.arguments);
        return T{};
    }
}
