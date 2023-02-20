#pragma once

namespace kengine::core::sort {
	template<typename... Comps, typename Registry>
	auto get_name_sorted_entities(Registry && r) noexcept;

	template<size_t MaxCount, typename... Comps, typename Registry>
	auto get_name_sorted_entities(Registry && r) noexcept;
}

#include "get_name_sorted_entities.inl"