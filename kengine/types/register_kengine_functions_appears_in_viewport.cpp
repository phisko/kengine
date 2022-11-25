#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/functions/appears_in_viewport.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types{
	void register_kengine_functions_appears_in_viewport(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::functions::appears_in_viewport'");
		kengine::register_components<kengine::functions::appears_in_viewport>(r);
	}
}