#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine data
#include "kengine/data/async_task.hpp"

namespace kengine {
	template<typename T, typename ... Args, typename Func>
	void start_async_task(entt::registry & r, entt::entity e, const data::async_task::string & task_name, Func && func) noexcept;

	template<typename T>
	bool process_async_results(entt::registry & r) noexcept;

	template<typename T, typename Func> // Func: void(entt::entity, T &&)
	bool process_async_results(entt::registry & r, Func && func) noexcept;
}

#include "async_helper.inl"