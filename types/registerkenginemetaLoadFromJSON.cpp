#include "helpers/registerTypeHelper.hpp"
#include "meta/LoadFromJSON.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaLoadFromJSON(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::LoadFromJSON'");
		kengine::registerComponents<kengine::meta::LoadFromJSON>(r);
	}
}