#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/functions/query_position.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_functions_query_position(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::functions::query_position'");
		kengine::register_everything<kengine::functions::query_position>(r);
	}
}