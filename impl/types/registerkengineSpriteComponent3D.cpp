#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

namespace kengine::types{
	void registerkengineSpriteComponent3D() noexcept {
		kengine::registerComponents<kengine::SpriteComponent3D>();

	}
}