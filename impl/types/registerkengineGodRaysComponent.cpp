#include "helpers/registerTypeHelper.hpp"
#include "data/GodRaysComponent.hpp"

namespace kengine::impl::types{
	void registerkengineGodRaysComponent() noexcept {
		kengine::registerComponents<kengine::GodRaysComponent>();

	}
}