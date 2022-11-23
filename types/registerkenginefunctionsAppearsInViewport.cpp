#include "helpers/registerTypeHelper.hpp"
#include "functions/AppearsInViewport.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsAppearsInViewport(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::functions::AppearsInViewport'");
		kengine::registerComponents<kengine::functions::AppearsInViewport>(r);
	}
}