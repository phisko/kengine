#include "helpers/registerTypeHelper.hpp"
#include "functions/OnClick.hpp"

namespace kengine::types{
	void registerkenginefunctionsOnClick() noexcept {
		kengine::registerComponents<kengine::functions::OnClick>();

	}
}