#include "helpers/registerTypeHelper.hpp"
#include "meta/Get.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaGet() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Get'");
		kengine::registerComponents<kengine::meta::Get>();
	}
}