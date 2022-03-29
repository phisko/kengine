#include "helpers/registerTypeHelper.hpp"
#include "meta/MatchString.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaMatchString() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::MatchString'");
		kengine::registerComponents<kengine::meta::MatchString>();

	}
}