#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/meta/for_each_entity.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_meta_for_each_entity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::meta::for_each_entity'");
		kengine::register_everything<true, kengine::meta::for_each_entity>(r);
	}
}