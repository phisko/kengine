#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/functions/on_mouse_captured.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_functions_on_mouse_captured(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::functions::on_mouse_captured'");
		kengine::register_everything<kengine::functions::on_mouse_captured>(r);
	}
}