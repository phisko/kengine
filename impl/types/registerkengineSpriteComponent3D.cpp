#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

namespace kengine::impl::types{
	void registerkengineSpriteComponent3D() noexcept {
		kengine::registerComponents<kengine::SpriteComponent3D>();

	}
}