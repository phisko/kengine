#pragma once

// stl
#include <future>

namespace kengine::async {
	template<typename T>
	struct result {
		std::future<T> future;
	};
}