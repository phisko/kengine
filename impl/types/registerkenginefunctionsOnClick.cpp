#include "helpers/registerTypeHelper.hpp"
#include "functions/OnClick.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsOnClick() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::functions::OnClick'");
		kengine::registerComponents<kengine::functions::OnClick>();
	}
}