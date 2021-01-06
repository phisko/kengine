#pragma once

namespace kengine {
	template<typename T>
	struct no {
		using CompType = T;
	};

	namespace impl {
		template<typename>
		struct is_not : std::false_type {};

		template<typename T>
		struct is_not<no<T>> : std::true_type {};
	}
}