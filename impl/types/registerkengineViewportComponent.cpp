#include "helpers/registerTypeHelper.hpp"
#include "data/ViewportComponent.hpp"

namespace kengine::impl::types{
	void registerkengineViewportComponent() noexcept {
		kengine::registerComponents<kengine::ViewportComponent>();

	}
}