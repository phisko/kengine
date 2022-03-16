#pragma once

#include <gtest/gtest.h>
#include "kengine.hpp"

struct KengineTest : ::testing::Test {
    KengineTest() noexcept {
        kengine::init();
    }

    ~KengineTest() noexcept {
        kengine::terminate();
    }
};