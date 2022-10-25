#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineTextComponent2D() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TextComponent2D'");
		kengine::registerComponents<kengine::TextComponent2D>();
	}
}