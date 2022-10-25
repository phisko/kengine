#include "helpers/registerTypeHelper.hpp"
#include "data/TransformComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineTransformComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TransformComponent'");
		kengine::registerComponents<kengine::TransformComponent>();
	}
}