#include "model.tests.hpp"

// kengine
#include "kengine/model/helpers/try_get.hpp"

TEST_F(model, try_get_entity) {
	EXPECT_EQ(kengine::model::try_get<int>(e), nullptr);
	EXPECT_EQ(*kengine::model::try_get<std::string>(e), "hello");
}

TEST_F(model, try_get_component) {
	const auto & comp = e.get<kengine::model::instance>();
	EXPECT_EQ(kengine::model::try_get<int>(r, comp), nullptr);
	EXPECT_EQ(*kengine::model::try_get<std::string>(r, comp), "hello");
}
