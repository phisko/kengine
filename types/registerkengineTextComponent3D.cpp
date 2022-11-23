#include "helpers/registerTypeHelper.hpp"
#include "data/TextComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineTextComponent3D(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::TextComponent3D'");
		kengine::registerComponents<kengine::TextComponent3D>(r);
	}
}