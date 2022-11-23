#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSkyBoxComponent(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::SkyBoxComponent'");
		kengine::registerComponents<kengine::SkyBoxComponent>(r);
	}
}