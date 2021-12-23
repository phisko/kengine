#include "helpers/registerTypeHelper.hpp"
#include "data/CameraComponent.hpp"

namespace kengine::types{
	void registerkengineCameraComponent() noexcept {
		kengine::registerComponents<kengine::CameraComponent>();

	}
}