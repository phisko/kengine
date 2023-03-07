#include "model.tests.hpp"

// kengine
#include "kengine/model/helpers/has.hpp"

TEST_F(model, has_entity) {
	EXPECT_TRUE(kengine::model::has<std::string>(e));
	EXPECT_FALSE(kengine::model::has<int>(e));
}

TEST_F(model, has_component) {
	const auto & comp = e.get<kengine::model::instance>();
	EXPECT_TRUE(kengine::model::has<std::string>(r, comp));
	EXPECT_FALSE(kengine::model::has<int>(r, comp));
}
