#include "helpers/registerTypeHelper.hpp"
#include "data/SpriteComponent.hpp"

namespace kengine::impl::types{
	void registerkengineSpriteComponent2D() noexcept {
		kengine::registerComponents<kengine::SpriteComponent2D>();

	}
}