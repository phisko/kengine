#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"

namespace kengine::types{
	void registerkengineSkyBoxComponent() noexcept {
		kengine::registerComponents<kengine::SkyBoxComponent>();

	}
}