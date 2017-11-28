#include <chrono>
#include <thread>
#include "gtest/gtest.h"
#include "EntityManager.hpp"

template<typename ...Systems>
struct SystemTest : testing::Test {
    SystemTest() {
        em.loadSystems<Systems...>();
    }

    void runTimes(std::size_t times) {
        const auto sleepTime = std::chrono::milliseconds(16);

        if (first) {
            first = false;
            em.execute(); // Reset timers
        }

        for (std::size_t i = 0; i < times; ++i) {
            std::this_thread::sleep_for(sleepTime);
            em.execute();
        }
    }

    kengine::EntityManager em;
    bool first = true;
};