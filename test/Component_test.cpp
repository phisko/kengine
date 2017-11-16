#include "gtest/gtest.h"
#include "GameObject.hpp"

struct ComponentTest : testing::Test
{
    struct A : kengine::Component<A, std::string>
    {
        std::string toString() const noexcept final { return "{type:A}"; }

        void handle(const std::string &str) { msg = str; }

        std::string msg;
    };

    struct B : kengine::Component<B>
    {
        std::string toString() const noexcept final { return "{type:B}"; }

        void work() { send(std::string("Message")); }
    };
};

TEST_F(ComponentTest, GetType)
{
    A a;
    A a2;
    B b;
    EXPECT_EQ(a.getType(), a2.getType());
    EXPECT_NE(a.getType(), b.getType());
}

TEST_F(ComponentTest, SenderRececeiver)
{
    kengine::GameObject go("name");
    auto &a = go.attachComponent<A>();
    auto &b = go.attachComponent<B>();

    b.work();
    EXPECT_EQ(a.msg, "Message");
}

TEST_F(ComponentTest, ToString)
{
    EXPECT_EQ(putils::to_string(A()), "{type:A}");
}

TEST_F(ComponentTest, IsComponent)
{
    EXPECT_TRUE(kengine::is_component<A>::value);
    EXPECT_FALSE(kengine::is_component<int>::value);
}
