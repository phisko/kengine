// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine async
#include "kengine/async/data/async_task.hpp"
#include "kengine/async/helpers/async_helper.hpp"

TEST(async_helper, start_async_task_component_created) {
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

TEST(async_helper, start_async_task_formatted_name) {
	entt::registry r;
	const auto e = r.create();
	kengine::async::start_task(
		r, e,
		kengine::async::task::string("%s %d", "hello", 0),
		std::async(std::launch::deferred, [] {})
	);
	EXPECT_EQ(r.get<kengine::async::task>(e).name, "hello 0");
}

TEST(async_helper, process_async_results_deferred_return_value) {
	entt::registry r;
	const auto e = r.create();
	kengine::async::start_task(
		r, e,
		kengine::async::task::string("%s %d", "hello", 0),
		std::async(std::launch::deferred, [] {
			return 42;
		})
	);

	int processed = 0;
	const auto done = kengine::async::process_results<int>(r, [&](entt::entity, int result) {
		++processed;
		EXPECT_EQ(result, 42);
	});
	EXPECT_TRUE(done);
	EXPECT_EQ(processed, 1);
}

TEST(async_helper, process_async_results_deferred_side_effect) {
	entt::registry r;
	const auto e = r.create();

	int result = 0;
	kengine::async::start_task(
		r, e,
		kengine::async::task::string("%s %d", "hello", 0),
		std::async(std::launch::deferred, [&] {
			result = 42;
			return 0;
		})
	);

	int processed = 0;
	const auto done = kengine::async::process_results<int>(r, [&](entt::entity, int) {
		++processed;
	});
	EXPECT_TRUE(done);
	EXPECT_EQ(processed, 1);
	EXPECT_EQ(result, 42);
}

TEST(async_helper, process_async_results_async_return_value) {
	entt::registry r;
	const auto e = r.create();
	kengine::async::start_task(
		r, e,
		kengine::async::task::string("%s %d", "hello", 0),
		std::async(std::launch::async, [] {
			return 42;
		})
	);

	int processed = 0;
	while (!kengine::async::process_results<int>(r, [&](entt::entity, int result) {
		++processed;
		EXPECT_EQ(result, 42);
	}))
		;
	EXPECT_EQ(processed, 1);
}

TEST(async_helper, process_async_results_async_side_effect) {
	entt::registry r;
	const auto e = r.create();

	int result = 0;
	kengine::async::start_task(
		r, e,
		kengine::async::task::string("%s %d", "hello", 0),
		std::async(std::launch::async, [&] {
			result = 42;
			return 0;
		})
	);

	int processed = 0;
	while (!kengine::async::process_results<int>(r, [&](entt::entity, int) {
		++processed;
	}))
		;
	EXPECT_EQ(processed, 1);
	EXPECT_EQ(result, 42);
}
