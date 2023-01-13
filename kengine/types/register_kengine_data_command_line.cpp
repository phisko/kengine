#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/data/command_line.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_command_line(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::command_line'");
		kengine::register_everything<kengine::data::command_line>(r);
	}
}