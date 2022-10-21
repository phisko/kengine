#pragma once

#include <gtest/gtest.h>
#include "kengine.hpp"
#include "scoped_setter.hpp"

struct KengineTest : ::testing::Test {
    KengineTest() noexcept {
        kengine::init();
    }

    ~KengineTest() noexcept {
        kengine::terminate();
    }
};