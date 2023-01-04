#include "register_emplace_or_replace.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/emplace_or_replace.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_emplace_or_replace(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::emplace_or_replace, Comps...>(
			r, [](const auto t, entt::handle e, const void * comp) noexcept {
				using type = putils_wrapped_type(t);
				if (comp) {
					const auto typed_comp = static_cast<const type *>(comp);
					e.emplace_or_replace<type>(*typed_comp);
				}
				else
					e.emplace_or_replace<type>();
			}
		);

		register_meta_component_implementation<meta::emplace_or_replace_move, Comps...>(
			r, [](const auto t, entt::handle e, void * comp) noexcept {
				using type = putils_wrapped_type(t);
				if (comp) {
					const auto typed_comp = static_cast<type *>(comp);
					e.emplace_or_replace<type>(std::move(*typed_comp));
				}
				else
					e.emplace_or_replace<type>();
			}
		);
	}
}