#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/meta/load_from_json.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_meta_load_from_json(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::meta::load_from_json'");
		kengine::register_everything<kengine::meta::load_from_json>(r);
	}
}