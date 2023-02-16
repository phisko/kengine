#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::async {
	template<typename T>
	bool process_results(entt::registry & r) noexcept;

	template<typename T, std::invocable<entt::entity, T &&> Func>
	bool process_results(entt::registry & r, Func && func) noexcept;
}

#include "process_results.inl"