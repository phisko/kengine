#include "helpers/registerTypeHelper.hpp"
#include "meta/Copy.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaCopy(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::Copy'");
		kengine::registerComponents<kengine::meta::Copy>(r);
	}
}