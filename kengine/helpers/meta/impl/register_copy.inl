#include "register_copy.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/copy.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_copy(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation_with_predicate<meta::copy, std::is_copy_constructible, Comps...>(
			r, [](const auto t, entt::const_handle src, entt::handle dst) noexcept {
				using type = putils_wrapped_type(t);
				if constexpr (!std::is_empty<type>()) {
					const auto & src_component = src.get<type>();
					dst.emplace_or_replace<type>(src_component);
				}
				else
					dst.emplace<type>();
			}
		);
	}
}
