#include "helpers/registerTypeHelper.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineImGuiToolComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ImGuiToolComponent'");
		kengine::registerComponents<kengine::ImGuiToolComponent>();
	}
}