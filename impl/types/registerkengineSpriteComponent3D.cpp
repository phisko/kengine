#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSpriteComponent3D() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SpriteComponent3D'");
		kengine::registerComponents<kengine::SpriteComponent3D>();
	}
}