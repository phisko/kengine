#include "helpers/registerTypeHelper.hpp"
#include "meta/Get.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaGet() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Get'");
		kengine::registerComponents<kengine::meta::Get>();
	}
}