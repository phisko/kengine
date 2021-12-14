#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"

namespace kengine::impl::types{
	void registerkengineSkyBoxComponent() noexcept {
		kengine::registerComponents<kengine::SkyBoxComponent>();

	}
}