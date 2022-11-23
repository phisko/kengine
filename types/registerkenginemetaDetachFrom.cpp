#include "helpers/registerTypeHelper.hpp"
#include "meta/DetachFrom.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaDetachFrom(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::DetachFrom'");
		kengine::registerComponents<kengine::meta::DetachFrom>(r);
	}
}