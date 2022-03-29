#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineImGuiToolComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ImGuiToolComponent'");
		kengine::registerComponents<kengine::ImGuiToolComponent>();

	}
}