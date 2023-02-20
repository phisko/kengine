#pragma once

namespace kengine::core::sort {
	// Returns a container of tuple<Entity, Comps *...>
	// Pred: bool(tuple<Entity, Comps*...> lhs, tuple<Entity, Comps*...> rhs) = lhs < rhs
	template<typename... Comps, typename Registry, typename Pred>
	auto get_sorted_entities(Registry && r, Pred && pred) noexcept;

	// Returns a stack-allocated putils::vector instead of std::vector
	template<size_t MaxCount, typename... Comps, typename Registry, typename Pred>
	auto get_sorted_entities(Registry && r, Pred && pred) noexcept;
}

#include "get_sorted_entities.inl"