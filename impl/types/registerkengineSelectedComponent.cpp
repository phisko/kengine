#include "helpers/registerTypeHelper.hpp"
#include "data/SelectedComponent.hpp"

namespace kengine::impl::types{
	void registerkengineSelectedComponent() noexcept {
		kengine::registerComponents<kengine::SelectedComponent>();

	}
}