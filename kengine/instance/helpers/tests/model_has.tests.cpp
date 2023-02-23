#include "instance.tests.hpp"

// kengine instance
#include "kengine/instance/helpers/model_has.hpp"

TEST_F(instance, model_has_entity) {
	EXPECT_TRUE(kengine::instance::model_has<std::string>(e));
	EXPECT_FALSE(kengine::instance::model_has<int>(e));
}

TEST_F(instance, model_has_component) {
	const auto & comp = e.get<kengine::instance::instance>();
	EXPECT_TRUE(kengine::instance::model_has<std::string>(r, comp));
	EXPECT_FALSE(kengine::instance::model_has<int>(r, comp));
}
