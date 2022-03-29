#include "helpers/registerTypeHelper.hpp"
#include "data/ModelAnimationComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineModelAnimationComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelAnimationComponent'");
		kengine::registerComponents<kengine::ModelAnimationComponent>();

	}
}