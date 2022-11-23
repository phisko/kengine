// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine data
#include "data/KeepAlive.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/isRunning.hpp"
#include "helpers/mainLoop.hpp"

TEST(mainLoop, run) {
	entt::registry r;

    size_t calls = 0;

	const auto e = r.create();
	r.emplace<kengine::KeepAlive>(e);
	r.emplace<kengine::functions::Execute>(
		e, [&](float deltaTime) {
			++calls;
			kengine::stopRunning(r);
		}
	);

    kengine::mainLoop::run(r);
    EXPECT_EQ(calls, 1);
}

TEST(mainLoop, timeModulated) {
	entt::registry r;

    size_t calls = 0;

	const auto e = r.create();
	r.emplace<kengine::KeepAlive>(e);
	r.emplace<kengine::functions::Execute>(
		e, [&](float deltaTime) {
			++calls;
			EXPECT_NEAR(deltaTime, 0.f, .001f);
			kengine::stopRunning(r);
		}
	);

    kengine::mainLoop::timeModulated::run(r);
    EXPECT_EQ(calls, 1);
}