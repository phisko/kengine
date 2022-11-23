#include "sortHelper.hpp"

// meta
#include "meta/fwd.hpp"

// putils
#include "vector.hpp"

// kengine data
#include "data/NameComponent.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine::sortHelper {
	namespace impl {
		template<typename PointerTuple, typename RefTuple>
		void setImpl(PointerTuple & p, const RefTuple & r, std::index_sequence<>) noexcept {}

		template<typename PointerTuple, typename RefTuple, size_t I, size_t ...Is>
		void setImpl(PointerTuple & p, const RefTuple & r, std::index_sequence<I, Is...>) noexcept {
			std::get<I + 1>(p) = &std::get<I + 1>(r);
			setImpl(p, r, std::index_sequence<Is...>());
		}

		template<typename PointerTuple, typename RefTuple, size_t ...Is>
		void set(PointerTuple & p, const RefTuple & r, std::index_sequence<Is...> is) noexcept {
			std::get<0>(p) = std::get<0>(r); // 0 is Entity
			setImpl(p, r, is);
		}
	}

	template<size_t MaxCount, typename ... Comps, typename Registry, typename Pred>
	auto getSortedEntities(Registry && r, Pred && pred) noexcept {
		KENGINE_PROFILING_SCOPE;

		using Type = std::tuple<entt::entity, Comps *...>;

		using Ret = std::conditional_t<
			MaxCount == 0,
			std::vector<Type>,
			putils::vector<Type, MaxCount>
		>;
		Ret ret;

		for (const auto & t : FWD(r).template view<Comps...>().each()) {
			if constexpr (putils::is_vector<Ret>::value)
				if (ret.full())
					break;
			ret.emplace_back();
			impl::set(ret.back(), t, std::make_index_sequence<sizeof...(Comps)>());
		}
		std::sort(ret.begin(), ret.end(), FWD(pred));

		return ret;
	}

	template<typename ... Comps, typename Registry, typename Pred>
	auto getSortedEntities(Registry && r, Pred && pred) noexcept {
		return getSortedEntities<0, Comps...>(FWD(r), FWD(pred));
	}

	template<size_t MaxCount, typename ... Comps, typename Registry>
	auto getNameSortedEntities(Registry && r) noexcept {
		KENGINE_PROFILING_SCOPE;

		return getSortedEntities<MaxCount, const NameComponent, Comps...>(
			FWD(r), [](const auto & lhs, const auto & rhs) noexcept {
				return strcmp(std::get<1>(lhs)->name.c_str(), std::get<1>(rhs)->name.c_str()) < 0;
			}
		);
	}

	template<typename ... Comps, typename Registry>
	auto getNameSortedEntities(Registry && r) noexcept {
		return getNameSortedEntities<0, Comps...>(FWD(r));
	}
}
