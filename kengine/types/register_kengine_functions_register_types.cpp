#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/functions/register_types.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_functions_register_types(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::functions::register_types'");
		kengine::register_everything<true, kengine::functions::register_types>(r);
	}
}