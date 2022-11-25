#pragma once

// stl
#include <memory>

namespace kengine {
	template<typename T, void (*FreeFunc)(T *)>
	struct deleter {
		void operator()(T * ptr) noexcept { FreeFunc(ptr); }
	};

	template<typename T, void(*FreeFunc)(T *)>
	using unique_ptr = std::unique_ptr<T, deleter<T, FreeFunc>>;
}