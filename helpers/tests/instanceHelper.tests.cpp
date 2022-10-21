#include "tests/KengineTest.hpp"
#include "helpers/instanceHelper.hpp"

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
	EXPECT_EQ(kengine::instanceHelper::getModel<std::string>(instance), "hello");
}

TEST_F(instanceHelper, getModel_Entity_missing) {
	EXPECT_DEATH(kengine::instanceHelper::getModel<int>(instance), ".*");
}

TEST_F(instanceHelper, getModel_Component) {
    const auto & comp = instance.get<kengine::InstanceComponent>();
    EXPECT_EQ(kengine::instanceHelper::getModel<std::string>(comp), "hello");
}

TEST_F(instanceHelper, getModel_Component_missing) {
	const auto & comp = instance.get<kengine::InstanceComponent>();
	EXPECT_DEATH(kengine::instanceHelper::getModel<int>(comp), ".*");
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
