#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiToolComponent.hpp"

namespace kengine::impl::types{
	void registerkengineImGuiToolComponent() noexcept {
		kengine::registerComponents<kengine::ImGuiToolComponent>();

	}
}