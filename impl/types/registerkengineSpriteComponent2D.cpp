#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSpriteComponent2D() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SpriteComponent2D'");
		kengine::registerComponents<kengine::SpriteComponent2D>();
	}
}