#include "gtest/gtest.h"
#include "EntityManager.hpp"

struct EntityManagerTest : testing::Test
{
    kengine::EntityManager em;

    struct A : kengine::Component<A>
    {
        std::string toString() const noexcept final { return ""; }
    };
};

TEST_F(EntityManagerTest, CreateEntityTemplate)
{
    const auto &go = em.createEntity<kengine::GameObject>("name");
    EXPECT_EQ(go.getName(), "name");
    EXPECT_TRUE(em.hasEntity("name"));
}

TEST_F(EntityManagerTest, CreateEntityFactory)
{
    em.getFactory<kengine::ExtensibleFactory>().registerType<kengine::GameObject>();
    const auto &go = em.createEntity("GameObject", "name");
    EXPECT_EQ(go.getName(), "name");
    EXPECT_TRUE(em.hasEntity("name"));
}

TEST_F(EntityManagerTest, CreateEntityNoNameTemplate)
{
    const auto &go = em.createEntity<kengine::GameObject>();
    EXPECT_EQ(go.getName(), "GameObject0");
    EXPECT_TRUE(em.hasEntity("GameObject0"));

    const auto &other = em.createEntity<kengine::GameObject>();
    EXPECT_EQ(other.getName(), "GameObject1");
    EXPECT_TRUE(em.hasEntity("GameObject1"));
}

TEST_F(EntityManagerTest, CreateEntityFactoryNoNameFactory)
{
    em.getFactory<kengine::ExtensibleFactory>().registerType<kengine::GameObject>();

    const auto &go = em.createEntity("GameObject");
    EXPECT_EQ(go.getName(), "GameObject0");
    EXPECT_TRUE(em.hasEntity("GameObject0"));

    const auto &other = em.createEntity("GameObject");
    EXPECT_EQ(other.getName(), "GameObject1");
    EXPECT_TRUE(em.hasEntity("GameObject1"));
}

TEST_F(EntityManagerTest, RemoveEntityRef)
{
    auto &go = em.createEntity<kengine::GameObject>("name");
    em.removeEntity(go);
    EXPECT_FALSE(em.hasEntity("name"));
    auto &other = em.createEntity<kengine::GameObject>("name");
    EXPECT_EQ(other.getName(), "name");
    EXPECT_TRUE(em.hasEntity("name"));
}

TEST_F(EntityManagerTest, RemoveEntityName)
{
    auto &go = em.createEntity<kengine::GameObject>("name");
    em.removeEntity("name");
    EXPECT_FALSE(em.hasEntity("name"));
    auto &other = em.createEntity<kengine::GameObject>("name");
    EXPECT_EQ(other.getName(), "name");
    EXPECT_TRUE(em.hasEntity("name"));
}

TEST_F(EntityManagerTest, RemoveEntityBad)
{
    em.createEntity<kengine::GameObject>("name");
    EXPECT_THROW(em.removeEntity("stupid"), std::out_of_range);
}

TEST_F(EntityManagerTest, GetEntity)
{
    em.createEntity<kengine::GameObject>("name");
    auto &go = em.getEntity("name");
    EXPECT_EQ(go.getName(), "name");
}

TEST_F(EntityManagerTest, GetEntityBad)
{
    em.createEntity<kengine::GameObject>("name");
    EXPECT_THROW(em.getEntity("stupid"), std::out_of_range);
}

TEST_F(EntityManagerTest, GetGameObjects)
{
    em.createEntity<kengine::GameObject>("name");
    const auto &objects = em.getGameObjects();
    EXPECT_EQ(objects.size(), 1);
    EXPECT_EQ(objects[0]->getName(), "name");
}

TEST_F(EntityManagerTest, GetGameObjectsWith)
{
    auto &go = em.createEntity<kengine::GameObject>("name");
    em.createEntity<kengine::GameObject>("other");
    go.attachComponent<A>();
    const auto &objects = em.getGameObjects<A>();
    EXPECT_EQ(objects.size(), 1);
    EXPECT_EQ(objects[0]->getName(), "name");
}

TEST_F(EntityManagerTest, AddLink)
{
    auto &parent = em.createEntity<kengine::GameObject>();
    auto &child = em.createEntity<kengine::GameObject>();
    em.addLink(parent, child);
    EXPECT_EQ(&parent, &em.getParent(child));
}

TEST_F(EntityManagerTest, GetParentBad)
{
    auto &go = em.createEntity<kengine::GameObject>();
    EXPECT_THROW(em.getParent(go), std::out_of_range);
}
