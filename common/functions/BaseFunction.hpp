#pragma once

namespace kengine {
	namespace functions {
		template<typename>
		struct BaseFunction;

		template<typename Ret, typename ... Args>
		struct BaseFunction<Ret(Args...)> {
			using Signature = Ret(*)(Args...);
			Signature funcPtr;

			bool operator==(nullptr_t) const { return funcPtr == nullptr; }
			bool operator!=(nullptr_t) const { return funcPtr != nullptr; }

			Ret operator()(Args... args) const { return funcPtr(args...); }
			Ret call(Args ... args) const { return funcPtr(args...); }
		};
	}
}

