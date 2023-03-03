// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine async
#include "kengine/async/data/task.hpp"
#include "kengine/async/helpers/start_task.hpp"

TEST(async, start_task_component_created) {
	entt::registry r;
	const auto e = r.create();
	kengine::async::start_task(
		r, e,
		"task_name",
		std::async(std::launch::deferred, [] {})
	);

	EXPECT_TRUE(r.all_of<kengine::async::task>(e));
	EXPECT_EQ(r.get<kengine::async::task>(e).name, "task_name");

	const auto view = r.view<kengine::async::task>();
	EXPECT_EQ(view.size(), 1);
	EXPECT_EQ(view.front(), e);
}

TEST(async, start_task_formatted_name) {
	entt::registry r;
	const auto e = r.create();
	kengine::async::start_task(
		r, e,
		kengine::async::task::string("{} {}", "hello", 0),
		std::async(std::launch::deferred, [] {})
	);
	EXPECT_EQ(r.get<kengine::async::task>(e).name, "hello 0");
}
