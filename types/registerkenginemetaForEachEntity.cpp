#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaForEachEntity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::ForEachEntity'");
		kengine::registerComponents<kengine::meta::ForEachEntity>(r);
	}
}