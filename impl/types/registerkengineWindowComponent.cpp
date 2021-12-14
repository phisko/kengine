#include "helpers/registerTypeHelper.hpp"
#include "data/WindowComponent.hpp"

namespace kengine::impl::types{
	void registerkengineWindowComponent() noexcept {
		kengine::registerComponents<kengine::WindowComponent>();

	}
}