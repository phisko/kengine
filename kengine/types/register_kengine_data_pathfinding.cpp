#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/data/pathfinding.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_pathfinding(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::pathfinding'");
		kengine::register_everything<kengine::data::pathfinding>(r);
	}
}