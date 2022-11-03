#pragma once

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine.hpp"

struct KengineTest : ::testing::Test {
    KengineTest() noexcept {
        kengine::init();
    }

    ~KengineTest() noexcept {
        kengine::terminate();
    }
};