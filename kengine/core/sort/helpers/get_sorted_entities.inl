#include "get_sorted_entities.hpp"

// entt
#include <entt/entity/entity.hpp>

// meta
#include "putils/meta/fwd.hpp"

// putils
#include "putils/vector.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::core::sort {
	namespace impl {
		template<typename PointerTuple, typename RefTuple>
		void set_impl(PointerTuple & p, const RefTuple & r, std::index_sequence<>) noexcept {}

		template<typename PointerTuple, typename RefTuple, size_t I, size_t... Is>
		void set_impl(PointerTuple & p, const RefTuple & r, std::index_sequence<I, Is...>) noexcept {
			std::get<I + 1>(p) = &std::get<I + 1>(r);
			set_impl(p, r, std::index_sequence<Is...>());
		}

		template<typename PointerTuple, typename RefTuple, size_t... Is>
		void set(PointerTuple & p, const RefTuple & r, std::index_sequence<Is...> is) noexcept {
			std::get<0>(p) = std::get<0>(r); // 0 is Entity
			set_impl(p, r, is);
		}
	}

	template<size_t MaxCount, typename... Comps, typename Registry, typename Pred>
	auto get_sorted_entities(Registry && r, Pred && pred) noexcept {
		KENGINE_PROFILING_SCOPE;

		using tuple_type = std::tuple<entt::entity, Comps *...>;

		using return_type = std::conditional_t<
			MaxCount == 0,
			std::vector<tuple_type>,
			putils::vector<tuple_type, MaxCount>>;
		return_type ret;

		for (const auto & t : FWD(r).template view<Comps...>().each()) {
			if constexpr (putils::is_vector<return_type>::value)
				if (ret.full())
					break;
			ret.emplace_back();
			impl::set(ret.back(), t, std::make_index_sequence<sizeof...(Comps)>());
		}
		std::ranges::sort(ret, FWD(pred));

		return ret;
	}

	template<typename... Comps, typename Registry, typename Pred>
	auto get_sorted_entities(Registry && r, Pred && pred) noexcept {
		kengine_log(r, very_verbose, "core_sort", "Getting sorted entities");
		return get_sorted_entities<0, Comps...>(FWD(r), FWD(pred));
	}
}
