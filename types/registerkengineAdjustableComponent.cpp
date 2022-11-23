#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineAdjustableComponent(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::AdjustableComponent'");
		kengine::registerComponents<kengine::AdjustableComponent>(r);
	}
}