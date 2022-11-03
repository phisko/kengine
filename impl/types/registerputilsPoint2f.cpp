#include "helpers/registerTypeHelper.hpp"
#include "Point.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerputilsPoint2f() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'putils::Point2f'");
		kengine::registerTypes<putils::Point2f>();
	}
}