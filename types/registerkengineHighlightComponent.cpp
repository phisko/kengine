#include "helpers/registerTypeHelper.hpp"
#include "data/HighlightComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineHighlightComponent(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::HighlightComponent'");
		kengine::registerComponents<kengine::HighlightComponent>(r);
	}
}