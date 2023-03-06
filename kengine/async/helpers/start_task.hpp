#pragma once

// stl
#include <concepts>
#include <future>

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/async/data/task.hpp"

namespace kengine::async {
	template<typename T>
	void start_task(entt::registry & r, entt::entity e, const task::string & task_name, std::future<T> && future) noexcept;
}

#include "start_task.inl"