#include "get_name_sorted_entities.hpp"

#include "get_sorted_entities.hpp"

namespace kengine::core::sort {
	template<size_t MaxCount, typename... Comps, typename Registry>
	auto get_name_sorted_entities(Registry && r) noexcept {
		KENGINE_PROFILING_SCOPE;

		return get_sorted_entities<MaxCount, const data::name, Comps...>(
			FWD(r), [](const auto & lhs, const auto & rhs) noexcept {
				return strcmp(std::get<1>(lhs)->name.c_str(), std::get<1>(rhs)->name.c_str()) < 0;
			}
		);
	}

	template<typename... Comps, typename Registry>
	auto get_name_sorted_entities(Registry && r) noexcept {
		kengine_log(r, very_verbose, "core_sort", "Getting name sorted entities");
		return get_name_sorted_entities<0, Comps...>(FWD(r));
	}
}