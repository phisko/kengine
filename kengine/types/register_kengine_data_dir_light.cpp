#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/data/light.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_dir_light(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::dir_light'");
		kengine::register_everything<kengine::data::dir_light>(r);
	}
}