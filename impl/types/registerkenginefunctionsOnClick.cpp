#include "helpers/registerTypeHelper.hpp"
#include "functions/OnClick.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsOnClick() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::functions::OnClick'");
		kengine::registerComponents<kengine::functions::OnClick>();
	}
}