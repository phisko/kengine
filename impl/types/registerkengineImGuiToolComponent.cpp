#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiToolComponent.hpp"

namespace kengine::types{
	void registerkengineImGuiToolComponent() noexcept {
		kengine::registerComponents<kengine::ImGuiToolComponent>();

	}
}