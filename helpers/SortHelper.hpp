#pragma once

#include "EntityManager.hpp"
#include "data/NameComponent.hpp"

namespace kengine::SortHelper {
	// Returns a container of tuple<Entity, Comps *...>
	// Pred: bool(tuple<Entity, Comps*...> lhs, tuple<Entity, Comps*...> rhs) = lhs < rhs 
	template<typename ... Comps, typename Pred>
	auto getSortedEntities(EntityManager & em, Pred && pred);

	template<typename ... Comps>
	auto getNameSortedEntities(EntityManager & em);
}

namespace kengine::SortHelper {
	namespace detail {
		template<typename PointerTuple, typename RefTuple, size_t I, size_t ...Is>
		void setImpl(PointerTuple & p, const RefTuple & r, std::index_sequence<I, Is...>) {
			std::get<I + 1>(p) = &std::get<I + 1>(r);
			setImpl(p, r, std::index_sequence<Is...>());
		}

		template<typename PointerTuple, typename RefTuple>
		void setImpl(PointerTuple & p, const RefTuple & r, std::index_sequence<>) {}

		template<typename PointerTuple, typename RefTuple, size_t ...Is>
		void set(PointerTuple & p, const RefTuple & r, std::index_sequence<Is...> is) {
			std::get<0>(p) = std::get<0>(r); // 0 is Entity
			setImpl(p, r, is);
		}
	}

	template<typename ... Comps, typename Pred>
	auto getSortedEntities(EntityManager & em, Pred && pred) { 
		using Type = std::tuple<Entity, const Comps *...>;

		putils::vector<Type, KENGINE_COMPONENT_COUNT> ret;

		for (const auto & t : em.getEntities<Comps...>()) {
			ret.emplace_back();
			detail::set(ret.back(), t, std::make_index_sequence<sizeof...(Comps)>());
		}
		std::sort(ret.begin(), ret.end(), FWD(pred));

		return ret;
	}

	template<typename ... Comps>
	auto getNameSortedEntities(EntityManager & em) {
		return getSortedEntities<NameComponent, Comps...>(em,
			[](const auto & lhs, const auto & rhs) {
				return strcmp(std::get<1>(lhs)->name, std::get<1>(rhs)->name) < 0;
			}
		);
	}
}