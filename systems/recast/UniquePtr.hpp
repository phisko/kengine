#pragma once

#include <memory>

namespace kengine {
	template<typename T, void (*FreeFunc)(T *)>
	struct Deleter {
		void operator()(T * ptr) noexcept { FreeFunc(ptr); }
	};

	template<typename T, void(*FreeFunc)(T *)>
	using UniquePtr = std::unique_ptr<T, Deleter<T, FreeFunc>>;
}