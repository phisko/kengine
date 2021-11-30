#pragma once

namespace kengine::sortHelper {
	// Returns a container of tuple<Entity, Comps *...>
	// Pred: bool(tuple<Entity, Comps*...> lhs, tuple<Entity, Comps*...> rhs) = lhs < rhs 
	template<typename ... Comps, typename Pred>
	auto getSortedEntities(Pred && pred) noexcept;
	// Returns a stack-allocated putils::vector instead of std::vector
	template<size_t MaxCount, typename ... Comps, typename Pred>
	auto getSortedEntities(Pred && pred) noexcept;

	template<typename ... Comps>
	auto getNameSortedEntities() noexcept;
	template<size_t MaxCount, typename ... Comps>
	auto getNameSortedEntities() noexcept;
}

#include "sortHelper.inl"