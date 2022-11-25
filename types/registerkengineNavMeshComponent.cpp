#include "helpers/registerTypeHelper.hpp"
#include "data/NavMeshComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineNavMeshComponent(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::NavMeshComponent'");
		kengine::registerComponents<kengine::NavMeshComponent>(r);
	}
}