#include "helpers/registerTypeHelper.hpp"
#include "meta/MatchString.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaMatchString(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::MatchString'");
		kengine::registerComponents<kengine::meta::MatchString>(r);
	}
}