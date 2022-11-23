#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSpriteComponent3D(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::SpriteComponent3D'");
		kengine::registerComponents<kengine::SpriteComponent3D>(r);
	}
}