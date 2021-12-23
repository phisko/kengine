#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

namespace kengine::types{
	void registerkengineSpriteComponent2D() noexcept {
		kengine::registerComponents<kengine::SpriteComponent2D>();

	}
}