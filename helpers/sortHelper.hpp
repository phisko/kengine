#pragma once

namespace kengine::sortHelper {
	// Returns a container of tuple<Entity, Comps *...>
	// Pred: bool(tuple<Entity, Comps*...> lhs, tuple<Entity, Comps*...> rhs) = lhs < rhs 
	template<typename ... Comps, typename Registry, typename Pred>
	auto getSortedEntities(Registry && r, Pred && pred) noexcept;
	// Returns a stack-allocated putils::vector instead of std::vector
	template<size_t MaxCount, typename ... Comps, typename Registry, typename Pred>
	auto getSortedEntities(Registry && r, Pred && pred) noexcept;

	template<typename ... Comps, typename Registry>
	auto getNameSortedEntities(Registry && r) noexcept;
	template<size_t MaxCount, typename ... Comps, typename Registry>
	auto getNameSortedEntities(Registry && r) noexcept;
}

#include "sortHelper.inl"