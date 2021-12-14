#include "helpers/registerTypeHelper.hpp"
#include "functions/OnClick.hpp"

namespace kengine::impl::types{
	void registerkenginefunctionsOnClick() noexcept {
		kengine::registerComponents<kengine::functions::OnClick>();

	}
}