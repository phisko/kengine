#include "model.tests.hpp"

// kengine
#include "kengine/model/helpers/get.hpp"

TEST_F(model, get_entity) {
	EXPECT_EQ(kengine::model::get<std::string>(e), "hello");
}

TEST_F(model, get_entity_missing) {
#ifndef NDEBUG // entt might not assert in release
	EXPECT_DEATH(kengine::model::get<int>(e), ".*");
#endif
}

TEST_F(model, get_component) {
	const auto & comp = e.get<kengine::model::instance>();
	EXPECT_EQ(kengine::model::get<std::string>(r, comp), "hello");
}

TEST_F(model, get_component_missing) {
#ifndef NDEBUG // entt might not assert in release
	const auto & comp = e.get<kengine::model::instance>();
	EXPECT_DEATH(kengine::model::get<int>(r, comp), ".*");
#endif
}
