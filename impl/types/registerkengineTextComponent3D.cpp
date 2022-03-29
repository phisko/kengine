#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineTextComponent3D() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TextComponent3D'");
		kengine::registerComponents<kengine::TextComponent3D>();

	}
}