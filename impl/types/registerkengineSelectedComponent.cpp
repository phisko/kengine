#include "helpers/registerTypeHelper.hpp"
#include "data/SelectedComponent.hpp"

namespace kengine::types{
	void registerkengineSelectedComponent() noexcept {
		kengine::registerComponents<kengine::SelectedComponent>();

	}
}