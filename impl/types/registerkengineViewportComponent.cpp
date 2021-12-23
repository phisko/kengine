#include "helpers/registerTypeHelper.hpp"
#include "data/ViewportComponent.hpp"

namespace kengine::types{
	void registerkengineViewportComponent() noexcept {
		kengine::registerComponents<kengine::ViewportComponent>();

	}
}