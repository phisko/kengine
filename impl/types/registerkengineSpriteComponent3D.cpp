#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineSpriteComponent3D() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SpriteComponent3D'");
		kengine::registerComponents<kengine::SpriteComponent3D>();

	}
}