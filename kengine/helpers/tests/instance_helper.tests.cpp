// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "kengine/helpers/instance_helper.hpp"

struct instance_helper : ::testing::Test {
    instance_helper() noexcept {
		model = { r, r.create() };
		model.emplace<std::string>("hello");

        instance = { r, r.create() };
		instance.emplace<kengine::data::instance>(model);
    }

	entt::registry r;
    entt::handle model;
    entt::handle instance;
};

TEST_F(instance_helper, model_has_entity) {
    EXPECT_TRUE(kengine::instance_helper::model_has<std::string>(instance));
    EXPECT_FALSE(kengine::instance_helper::model_has<int>(instance));
}

TEST_F(instance_helper, model_has_component) {
    const auto & comp = instance.get<kengine::data::instance>();
    EXPECT_TRUE(kengine::instance_helper::model_has<std::string>(r, comp));
    EXPECT_FALSE(kengine::instance_helper::model_has<int>(r, comp));
}

TEST_F(instance_helper, get_model_entity) {
	EXPECT_EQ(kengine::instance_helper::get_model<std::string>(instance), "hello");
}

TEST_F(instance_helper, get_model_entity_missing) {
#ifndef NDEBUG // entt might not assert in release
	EXPECT_DEATH(kengine::instance_helper::get_model<int>(instance), ".*");
#endif
}

TEST_F(instance_helper, get_model_component) {
    const auto & comp = instance.get<kengine::data::instance>();
    EXPECT_EQ(kengine::instance_helper::get_model<std::string>(r, comp), "hello");
}

TEST_F(instance_helper, get_model_component_missing) {
#ifndef NDEBUG // entt might not assert in release
	const auto & comp = instance.get<kengine::data::instance>();
	EXPECT_DEATH(kengine::instance_helper::get_model<int>(r, comp), ".*");
#endif
}

TEST_F(instance_helper, try_get_model_entity) {
    EXPECT_EQ(kengine::instance_helper::try_get_model<int>(instance), nullptr);
    EXPECT_EQ(*kengine::instance_helper::try_get_model<std::string>(instance), "hello");
}

TEST_F(instance_helper, try_get_model_component) {
    const auto & comp = instance.get<kengine::data::instance>();
    EXPECT_EQ(kengine::instance_helper::try_get_model<int>(r, comp), nullptr);
    EXPECT_EQ(*kengine::instance_helper::try_get_model<std::string>(r, comp), "hello");
}
