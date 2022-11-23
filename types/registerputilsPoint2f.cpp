#include "helpers/registerTypeHelper.hpp"
#include "Point.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerputilsPoint2f(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'putils::Point2f'");
		kengine::registerTypes<putils::Point2f>(r);
	}
}