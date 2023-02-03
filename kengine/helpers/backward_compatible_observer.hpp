#pragma once

// stl
#include <functional>

// entt
#include <entt/entity/observer.hpp>

namespace kengine {
	template<typename... Comps>
	struct backward_compatible_observer {
		using callback_signature = void(entt::entity, Comps & ...);
		using callback_type = std::function<callback_signature>;

		backward_compatible_observer(entt::registry & r, const callback_type & callback) noexcept;
		void process() noexcept;

		entt::registry & r;
		entt::observer observer;
		callback_type callback;
	};
}

#include "backward_compatible_observer.inl"