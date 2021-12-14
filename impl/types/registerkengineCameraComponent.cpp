#include "helpers/registerTypeHelper.hpp"
#include "data/CameraComponent.hpp"

namespace kengine::impl::types{
	void registerkengineCameraComponent() noexcept {
		kengine::registerComponents<kengine::CameraComponent>();

	}
}