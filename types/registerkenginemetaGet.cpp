#include "helpers/registerTypeHelper.hpp"
#include "meta/Get.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaGet(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::meta::Get'");
		kengine::registerComponents<kengine::meta::Get>(r);
	}
}