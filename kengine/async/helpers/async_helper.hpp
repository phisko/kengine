#pragma once

// stl
#include <concepts>
#include <future>

// entt
#include <entt/entity/fwd.hpp>

// kengine async
#include "kengine/async/data/async_task.hpp"

namespace kengine::async {
	template<typename T>
	void start_task(entt::registry & r, entt::entity e, const task::string & task_name, std::future<T> && future) noexcept;

	template<typename T>
	bool process_results(entt::registry & r) noexcept;

	template<typename T, std::invocable<entt::entity, T &&> Func>
	bool process_results(entt::registry & r, Func && func) noexcept;
}

#include "async_helper.inl"