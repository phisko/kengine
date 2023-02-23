#include "instance.tests.hpp"

// kengine instance
#include "kengine/instance/helpers/get_model.hpp"

TEST_F(instance, get_model_entity) {
	EXPECT_EQ(kengine::instance::get_model<std::string>(e), "hello");
}

TEST_F(instance, get_model_entity_missing) {
#ifndef NDEBUG // entt might not assert in release
	EXPECT_DEATH(kengine::instance::get_model<int>(e), ".*");
#endif
}

TEST_F(instance, get_model_component) {
	const auto & comp = e.get<kengine::instance::instance>();
	EXPECT_EQ(kengine::instance::get_model<std::string>(r, comp), "hello");
}

TEST_F(instance, get_model_component_missing) {
#ifndef NDEBUG // entt might not assert in release
	const auto & comp = e.get<kengine::instance::instance>();
	EXPECT_DEATH(kengine::instance::get_model<int>(r, comp), ".*");
#endif
}
