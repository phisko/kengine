#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"

namespace kengine::types{
	void registerkengineTextComponent2D() noexcept {
		kengine::registerComponents<kengine::TextComponent2D>();

	}
}