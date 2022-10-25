#include "helpers/registerTypeHelper.hpp"
#include "Point.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerputilsPoint2f() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'putils::Point2f'");
		kengine::registerTypes<putils::Point2f>();
	}
}