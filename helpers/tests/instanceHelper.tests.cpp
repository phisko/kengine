// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "helpers/instanceHelper.hpp"

struct instanceHelper : ::testing::Test {
    instanceHelper() noexcept {
		model = { r, r.create() };
		model.emplace<std::string>("hello");

        instance = { r, r.create() };
		instance.emplace<kengine::InstanceComponent>(model);
    }

	entt::registry r;
    entt::handle model;
    entt::handle instance;
};

TEST_F(instanceHelper, modelHas_Entity) {
    EXPECT_TRUE(kengine::instanceHelper::modelHas<std::string>(instance));
    EXPECT_FALSE(kengine::instanceHelper::modelHas<int>(instance));
}

TEST_F(instanceHelper, modelHas_Component) {
    const auto & comp = instance.get<kengine::InstanceComponent>();
    EXPECT_TRUE(kengine::instanceHelper::modelHas<std::string>(r, comp));
    EXPECT_FALSE(kengine::instanceHelper::modelHas<int>(r, comp));
}

TEST_F(instanceHelper, getModel_Entity) {
	EXPECT_EQ(kengine::instanceHelper::getModel<std::string>(instance), "hello");
}

TEST_F(instanceHelper, getModel_Entity_missing) {
#ifndef NDEBUG // entt might not assert in release
	EXPECT_DEATH(kengine::instanceHelper::getModel<int>(instance), ".*");
#endif
}

TEST_F(instanceHelper, getModel_Component) {
    const auto & comp = instance.get<kengine::InstanceComponent>();
    EXPECT_EQ(kengine::instanceHelper::getModel<std::string>(r, comp), "hello");
}

TEST_F(instanceHelper, getModel_Component_missing) {
#ifndef NDEBUG // entt might not assert in release
	const auto & comp = instance.get<kengine::InstanceComponent>();
	EXPECT_DEATH(kengine::instanceHelper::getModel<int>(r, comp), ".*");
#endif
}

TEST_F(instanceHelper, tryGetModel_Entity) {
    EXPECT_EQ(kengine::instanceHelper::tryGetModel<int>(instance), nullptr);
    EXPECT_EQ(*kengine::instanceHelper::tryGetModel<std::string>(instance), "hello");
}

TEST_F(instanceHelper, tryGetModel_Component) {
    const auto & comp = instance.get<kengine::InstanceComponent>();
    EXPECT_EQ(kengine::instanceHelper::tryGetModel<int>(r, comp), nullptr);
    EXPECT_EQ(*kengine::instanceHelper::tryGetModel<std::string>(r, comp), "hello");
}
