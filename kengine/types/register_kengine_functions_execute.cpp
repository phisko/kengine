#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/functions/execute.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_functions_execute(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::functions::execute'");
		kengine::register_everything<true, kengine::functions::execute>(r);
	}
}