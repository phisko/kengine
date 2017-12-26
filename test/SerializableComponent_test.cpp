#include "gtest/gtest.h"
#include "SerializableComponent.hpp"

struct SerializableComponentTest : testing::Test {
    struct A : kengine::SerializableComponent<A> {
        A() : SerializableComponent<A>(
                std::make_pair(pmeta_reflectible_attribute(&A::msg))
        ) {}
        std::string msg = "msg";
    };
};

TEST_F(SerializableComponentTest, Test) {
    EXPECT_EQ(putils::to_string(A()), "{\"msg\": \"msg\"}");
    EXPECT_EQ(putils::to_string(A()), A().toString());
}
