#include "helpers/registerTypeHelper.hpp"
#include "meta/EditImGui.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaEditImGui(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::EditImGui'");
		kengine::registerComponents<kengine::meta::EditImGui>(r);
	}
}