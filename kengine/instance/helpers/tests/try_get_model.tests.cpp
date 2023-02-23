#include "instance.tests.hpp"

// kengine instance
#include "kengine/instance/helpers/try_get_model.hpp"

TEST_F(instance, try_get_model_entity) {
	EXPECT_EQ(kengine::instance::try_get_model<int>(e), nullptr);
	EXPECT_EQ(*kengine::instance::try_get_model<std::string>(e), "hello");
}

TEST_F(instance, try_get_model_component) {
	const auto & comp = e.get<kengine::instance::instance>();
	EXPECT_EQ(kengine::instance::try_get_model<int>(r, comp), nullptr);
	EXPECT_EQ(*kengine::instance::try_get_model<std::string>(r, comp), "hello");
}
