#include "helpers/registerTypeHelper.hpp"
#include "meta/Size.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaSize() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Size'");
		kengine::registerComponents<kengine::meta::Size>();

	}
}