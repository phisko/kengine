#include "gtest/gtest.h"
#include "EntityFactory.hpp"

struct EntityFactoryTest : testing::Test
{
    struct Example : kengine::EntityFactory
    {
        std::unique_ptr<kengine::GameObject> make(std::string_view type, std::string_view name) final
        {
            return std::make_unique<kengine::GameObject>(name);
        }
    };
};

TEST_F(EntityFactoryTest, SimpleMake)
{
    Example f;
    EXPECT_EQ(f.make("type", "name")->getName(), "name");
}

TEST_F(EntityFactoryTest, ExtensibleFactory)
{
    kengine::ExtensibleFactory f;
    f.registerType<kengine::GameObject>();
    EXPECT_EQ(f.make("GameObject", "name")->getName(), "name");
}

TEST_F(EntityFactoryTest, ExtensibleFactoryBad)
{
    kengine::ExtensibleFactory f;
    EXPECT_THROW(f.make("GameObject", "name"), std::out_of_range);
}

