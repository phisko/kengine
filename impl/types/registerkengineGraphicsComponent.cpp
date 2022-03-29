#include "helpers/registerTypeHelper.hpp"
#include "data/GraphicsComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineGraphicsComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::GraphicsComponent'");
		kengine::registerComponents<kengine::GraphicsComponent>();

	}
}