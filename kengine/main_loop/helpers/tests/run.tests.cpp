// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/main_loop/data/keep_alive.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/stop_running.hpp"
#include "kengine/main_loop/helpers/run.hpp"

TEST(main_loop, run) {
	entt::registry r;

	size_t calls = 0;

	const auto e = r.create();
	r.emplace<kengine::main_loop::keep_alive>(e);
	r.emplace<kengine::main_loop::execute>(
		e, [&](float delta_time) {
			++calls;
			kengine::main_loop::stop_running(r);
		}
	);

	kengine::main_loop::run(r);
	EXPECT_EQ(calls, 1);
}

TEST(main_loop, time_modulated) {
	entt::registry r;

	size_t calls = 0;

	const auto e = r.create();
	r.emplace<kengine::main_loop::keep_alive>(e);
	r.emplace<kengine::main_loop::execute>(
		e, [&](float delta_time) {
			++calls;
			EXPECT_NEAR(delta_time, 0.f, .001f);
			kengine::main_loop::stop_running(r);
		}
	);

	kengine::main_loop::time_modulated::run(r);
	EXPECT_EQ(calls, 1);
}