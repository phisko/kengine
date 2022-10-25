#include "helpers/registerTypeHelper.hpp"
#include "data/CameraComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineCameraComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::CameraComponent'");
		kengine::registerComponents<kengine::CameraComponent>();
	}
}