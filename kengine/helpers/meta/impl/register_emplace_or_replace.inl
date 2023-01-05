#include "register_emplace_or_replace.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "kengine/meta/emplace_or_replace.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_emplace_or_replace(entt::registry & r) noexcept {
		static const auto & g_r = r; // For use in kengine_assert

		// Helper function to handle non-movable types
		static const auto emplace_or_replace = [](const auto t, entt::handle e) noexcept {
			using type = putils_wrapped_type(t);
			if constexpr (std::is_move_assignable<type>()) // replace will attempt a move-assignment
				e.emplace_or_replace<type>();
			else {
				if (!e.all_of<type>())
					e.emplace<type>();
				else
					kengine_assert_failed(g_r, "Attempting to replace a non-movable %s", putils::reflection::get_class_name<type>());
			}
		};

		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::emplace_or_replace, Comps...>(
			r, [](const auto t, entt::handle e, const void * comp) noexcept {
				using type = putils_wrapped_type(t);
				if (comp) {
					if constexpr (std::is_copy_assignable<type>()) {
						const auto typed_comp = static_cast<const type *>(comp);
						e.emplace_or_replace<type>(*typed_comp);
					}
					else
						kengine_assert_failed(g_r, "Attempting to copy-assign a non-copyable %s", putils::reflection::get_class_name<type>());
				}
				else
					emplace_or_replace(t, e);
			}
		);

		register_meta_component_implementation<meta::emplace_or_replace_move, Comps...>(
			r, [](const auto t, entt::handle e, void * comp) noexcept {
				using type = putils_wrapped_type(t);
				if (comp) {
					if constexpr (std::is_move_assignable<type>()) {
						const auto typed_comp = static_cast<type *>(comp);
						e.emplace_or_replace<type>(std::move(*typed_comp));
					}
					else
						kengine_assert_failed(g_r, "Attempting to move-assign a non-movable %s", putils::reflection::get_class_name<type>());
				}
				else
					emplace_or_replace(t, e);
			}
		);
	}
}