#include "helpers/registerTypeHelper.hpp"
#include "functions/OnClick.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsOnClick(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::functions::OnClick'");
		kengine::registerComponents<kengine::functions::OnClick>(r);
	}
}