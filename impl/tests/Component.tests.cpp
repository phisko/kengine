#include "tests/KengineTest.hpp"
#include "impl/Component.hpp"

struct Component : KengineTest {};

TEST_F(Component, get) {
    auto & i = kengine::impl::Component<int>::get(0);
    i = 42;
    EXPECT_EQ(kengine::impl::Component<int>::get(0), 42);
}

TEST_F(Component, id) {
    EXPECT_EQ(kengine::impl::Component<int>::id(), 0);
    EXPECT_EQ(kengine::impl::Component<std::string>::id(), 1);
    EXPECT_EQ(kengine::impl::Component<int>::id(), 0);
    EXPECT_EQ(kengine::impl::Component<float>::id(), 2);
}