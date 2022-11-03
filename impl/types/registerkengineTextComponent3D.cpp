#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineTextComponent3D() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TextComponent3D'");
		kengine::registerComponents<kengine::TextComponent3D>();
	}
}