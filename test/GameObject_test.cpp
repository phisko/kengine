#include "gtest/gtest.h"
#include "GameObject.hpp"

struct GameObjectTest : testing::Test
{
    struct A : kengine::Component<A>
    {
        A() = default;
        A(std::string_view t) : msg(t.data()) {}

        std::string toString() const noexcept final { return ""; }

        std::string msg;
    };
};

TEST_F(GameObjectTest, Name)
{
    kengine::GameObject go("name");
    EXPECT_EQ(go.getName(), "name");
}

TEST_F(GameObjectTest, AttachComponent)
{
    kengine::GameObject go("name");
    const auto &a = go.attachComponent<A>();
    EXPECT_TRUE(go.hasComponent<A>());
}

TEST_F(GameObjectTest, AttachComponentParams)
{
    kengine::GameObject go("name");
    const auto &a = go.attachComponent<A>("msg");
    EXPECT_TRUE(go.hasComponent<A>());
    EXPECT_EQ(a.msg, "msg");
}

TEST_F(GameObjectTest, DetachComponentTemplate)
{
    kengine::GameObject go("name");
    const auto &a = go.attachComponent<A>();
    go.detachComponent<A>();
    EXPECT_FALSE(go.hasComponent<A>());
}

TEST_F(GameObjectTest, DetachComponentRef)
{
    kengine::GameObject go("name");
    const auto &a = go.attachComponent<A>();
    go.detachComponent(a);
    EXPECT_FALSE(go.hasComponent<A>());
}

TEST_F(GameObjectTest, GetComponent)
{
    kengine::GameObject go("name");
    go.attachComponent<A>("msg");
    auto &a = go.getComponent<A>();
    EXPECT_EQ(a.msg, "msg");
    a.msg = "new";
    EXPECT_EQ(a.msg, "new");
}

TEST_F(GameObjectTest, GetComponentConst)
{
    kengine::GameObject go("name");
    go.attachComponent<A>("msg");

    const auto &cgo = go;
    const auto &a = cgo.getComponent<A>();
    EXPECT_EQ(a.msg, "msg");
}