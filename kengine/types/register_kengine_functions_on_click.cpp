#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/functions/on_click.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types{
	void register_kengine_functions_on_click(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::functions::on_click'");
		kengine::register_components<kengine::functions::on_click>(r);
	}
}