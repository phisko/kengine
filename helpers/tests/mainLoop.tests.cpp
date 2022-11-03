#include "tests/KengineTest.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/mainLoop.hpp"

struct mainLoop : KengineTest {};

TEST_F(mainLoop, run) {
    size_t calls = 0;

    kengine::entities += [&](kengine::Entity & e) {
        e += kengine::functions::Execute{
            [&](float deltaTime) {
                ++calls;
                kengine::stopRunning();
            }
        };
    };

    kengine::mainLoop::run();
    EXPECT_EQ(calls, 1);
}

TEST_F(mainLoop, timeModulated) {
    size_t calls = 0;

    kengine::entities += [&](kengine::Entity & e) {
        e += kengine::functions::Execute{
            [&](float deltaTime) {
                ++calls;
                EXPECT_NEAR(deltaTime, 0.f, .001f);
                kengine::stopRunning();
            }
        };
    };

    kengine::mainLoop::timeModulated::run();
    EXPECT_EQ(calls, 1);
}