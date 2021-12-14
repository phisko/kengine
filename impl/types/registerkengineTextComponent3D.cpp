#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"

namespace kengine::impl::types{
	void registerkengineTextComponent3D() noexcept {
		kengine::registerComponents<kengine::TextComponent3D>();

	}
}