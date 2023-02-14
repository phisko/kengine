#pragma once

// stl
#include <functional>

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ProcessedTag, typename... Comps>
	struct new_entity_processor {
		using callback_signature = void(entt::entity e, Comps &... comps);
		using callback_type = std::function<callback_signature>;

		new_entity_processor(entt::registry & r, const callback_type & callback) noexcept;
		void process() noexcept;

		entt::registry & r;
		callback_type callback;
	};
}

#include "new_entity_processor.inl"