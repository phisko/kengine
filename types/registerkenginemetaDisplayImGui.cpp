#include "helpers/registerTypeHelper.hpp"
#include "meta/DisplayImGui.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaDisplayImGui(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::DisplayImGui'");
		kengine::registerComponents<kengine::meta::DisplayImGui>(r);
	}
}