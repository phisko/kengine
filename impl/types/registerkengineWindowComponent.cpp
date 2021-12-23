#include "helpers/registerTypeHelper.hpp"
#include "data/WindowComponent.hpp"

namespace kengine::types{
	void registerkengineWindowComponent() noexcept {
		kengine::registerComponents<kengine::WindowComponent>();

	}
}