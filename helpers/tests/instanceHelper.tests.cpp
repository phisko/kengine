#include "tests/KengineTest.hpp"
#include "helpers/instanceHelper.hpp"
#include "scoped_setter.hpp"

struct instanceHelper : KengineTest{
    instanceHelper() noexcept {
        model = kengine::entities += [](kengine::Entity & e) {
            e += std::string("hello");
        };

        instance = kengine::entities += [&](kengine::Entity & e) {
            e += kengine::InstanceComponent{ .model = model.id };
        };
    }

    kengine::Entity model;
    kengine::Entity instance;
};

TEST_F(instanceHelper, modelHas_Entity) {
    EXPECT_TRUE(kengine::instanceHelper::modelHas<std::string>(instance));
    EXPECT_FALSE(kengine::instanceHelper::modelHas<int>(instance));
}

TEST_F(instanceHelper, modelHas_Component) {
    const auto & comp = instance.get<kengine::InstanceComponent>();
    EXPECT_TRUE(kengine::instanceHelper::modelHas<std::string>(comp));
    EXPECT_FALSE(kengine::instanceHelper::modelHas<int>(comp));
}

TEST_F(instanceHelper, getModel_Entity) {
    static bool asserted = false;
    const auto _ = putils::setForScope(kengine::assertHelper::assertHandler, [](auto && ...) {
        asserted = true;
        return false;
    });

    asserted = false;
    kengine::instanceHelper::getModel<int>(instance);
    EXPECT_TRUE(asserted);

    asserted = false;
    EXPECT_EQ(kengine::instanceHelper::getModel<std::string>(instance), "hello");
    EXPECT_FALSE(asserted);
}

TEST_F(instanceHelper, getModel_Component) {
    static bool asserted = false;
    const auto _ = putils::setForScope(kengine::assertHelper::assertHandler, [](auto && ...) {
        asserted = true;
        return false;
    });

    const auto & comp = instance.get<kengine::InstanceComponent>();

    asserted = false;
    kengine::instanceHelper::getModel<int>(comp);
    EXPECT_TRUE(asserted);

    asserted = false;
    EXPECT_EQ(kengine::instanceHelper::getModel<std::string>(comp), "hello");
    EXPECT_FALSE(asserted);
}

TEST_F(instanceHelper, tryGetModel_Entity) {
    EXPECT_EQ(kengine::instanceHelper::tryGetModel<int>(instance), nullptr);
    EXPECT_EQ(*kengine::instanceHelper::tryGetModel<std::string>(instance), "hello");
}

TEST_F(instanceHelper, tryGetModel_Component) {
    const auto & comp = instance.get<kengine::InstanceComponent>();
    EXPECT_EQ(kengine::instanceHelper::tryGetModel<int>(comp), nullptr);
    EXPECT_EQ(*kengine::instanceHelper::tryGetModel<std::string>(comp), "hello");
}
