#pragma once

// stl
#include <concepts>
#include <future>

// entt
#include <entt/entity/fwd.hpp>

// kengine async
#include "kengine/async/data/async_task.hpp"

namespace kengine {
	template<typename T>
	void start_async_task(entt::registry & r, entt::entity e, const data::async_task::string & task_name, std::future<T> && future) noexcept;

	template<typename T>
	bool process_async_results(entt::registry & r) noexcept;

	template<typename T, std::invocable<entt::entity, T &&> Func>
	bool process_async_results(entt::registry & r, Func && func) noexcept;
}

#include "async_helper.inl"