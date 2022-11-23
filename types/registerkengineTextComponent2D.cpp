#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineTextComponent2D(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::TextComponent2D'");
		kengine::registerComponents<kengine::TextComponent2D>(r);
	}
}