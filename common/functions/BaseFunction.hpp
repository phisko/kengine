#pragma once

namespace kengine {
	namespace functions {
		template<typename>
		struct BaseFunction;

		template<typename Ret, typename ... Args>
		struct BaseFunction<Ret(Args...)> {
			using Signature = Ret(*)(Args...);
			Signature funcPtr;
		};
	}
}

