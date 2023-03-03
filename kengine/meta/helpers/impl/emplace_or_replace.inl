#include "emplace_or_replace.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine core
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	namespace impl {
		// Helper function to handle non-movable types
		template<typename T>
		static void emplace_or_replace(entt::handle e) noexcept {
			if constexpr (std::is_move_assignable<T>()) // replace will attempt a move-assignment
				e.emplace_or_replace<T>();
			else {
				if (!e.all_of<T>())
					e.emplace<T>();
				else
					kengine_assert_failed(*e.registry(), "Attempting to replace a non-movable {}", putils::reflection::get_class_name<T>());
			}
		}
	}

	template<typename T>
	void meta_component_implementation<emplace_or_replace, T>::function(entt::handle e, const void * comp) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::emplace_or_replace", "Emplacing or replacing {}'s {}", e, putils::reflection::get_class_name<T>());

		if (comp) {
			kengine_log(*e.registry(), very_verbose, "meta::emplace_or_replace", "Copying from argument");
			if constexpr (std::is_copy_assignable<T>()) {
				const auto typed_comp = static_cast<const T *>(comp);
				e.emplace_or_replace<T>(*typed_comp);
			}
			else
				kengine_assert_failed(*e.registry(), "Attempting to copy-assign a non-copyable {}", putils::reflection::get_class_name<T>());
		}
		else
			impl::emplace_or_replace<T>(e);
	}

	template<typename T>
	void meta_component_implementation<emplace_or_replace_move, T>::function(entt::handle e, void * comp) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::emplace_or_replace_move", "Emplacing or replacing {}'s {} by move", e, putils::reflection::get_class_name<T>());

		if (comp) {
			kengine_log(*e.registry(), very_verbose, "meta::emplace_or_replace_move", "Moving from argument");
			if constexpr (std::is_move_assignable<T>()) {
				const auto typed_comp = static_cast<T *>(comp);
				e.emplace_or_replace<T>(std::move(*typed_comp));
			}
			else
				kengine_assert_failed(*e.registry(), "Attempting to move-assign a non-movable {}", putils::reflection::get_class_name<T>());
		}
		else
			impl::emplace_or_replace<T>(e);
	}
}