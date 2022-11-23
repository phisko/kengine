#include "helpers/registerTypeHelper.hpp"
#include "meta/AttachTo.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaAttachTo(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::AttachTo'");
		kengine::registerComponents<kengine::meta::AttachTo>(r);
	}
}