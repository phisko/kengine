// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine main_loop
#include "kengine/main_loop/data/keep_alive.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/is_running.hpp"
#include "kengine/main_loop/helpers/main_loop.hpp"

TEST(main_loop, run) {
	entt::registry r;

	size_t calls = 0;

	const auto e = r.create();
	r.emplace<kengine::data::keep_alive>(e);
	r.emplace<kengine::functions::execute>(
		e, [&](float delta_time) {
			++calls;
			kengine::stop_running(r);
		}
	);

	kengine::main_loop::run(r);
	EXPECT_EQ(calls, 1);
}

TEST(main_loop, time_modulated) {
	entt::registry r;

	size_t calls = 0;

	const auto e = r.create();
	r.emplace<kengine::data::keep_alive>(e);
	r.emplace<kengine::functions::execute>(
		e, [&](float delta_time) {
			++calls;
			EXPECT_NEAR(delta_time, 0.f, .001f);
			kengine::stop_running(r);
		}
	);

	kengine::main_loop::time_modulated::run(r);
	EXPECT_EQ(calls, 1);
}