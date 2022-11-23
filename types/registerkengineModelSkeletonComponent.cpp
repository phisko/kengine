#include "helpers/registerTypeHelper.hpp"
#include "data/ModelSkeletonComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineModelSkeletonComponent(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::ModelSkeletonComponent'");
		kengine::registerComponents<kengine::ModelSkeletonComponent>(r);
	}
}