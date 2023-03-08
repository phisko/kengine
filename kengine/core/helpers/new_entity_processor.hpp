#pragma once

// stl
#include <functional>

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	namespace detail {
		template<typename... Comps>
		struct any_is_empty;

		template<typename T, typename... Rest>
		struct any_is_empty<T, Rest...> : std::conditional_t<
			std::is_empty_v<T>,
			std::true_type,
			any_is_empty<Rest...>
		> {};

		template<>
		struct any_is_empty<> : std::false_type {};
	}

	template<typename ProcessedTag, typename... Comps>
	struct new_entity_processor {
		using callback_signature = std::conditional_t<
		    detail::any_is_empty<Comps...>::value,
			void(entt::entity e),
			void(entt::entity e, Comps &... comps)
		>;
		using callback_type = std::function<callback_signature>;

		new_entity_processor(entt::registry & r, const callback_type & callback) noexcept;
		void process() noexcept;

		entt::registry & r;
		callback_type callback;
	};
}

#include "new_entity_processor.inl"