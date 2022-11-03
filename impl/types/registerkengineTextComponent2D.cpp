#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineTextComponent2D() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TextComponent2D'");
		kengine::registerComponents<kengine::TextComponent2D>();
	}
}