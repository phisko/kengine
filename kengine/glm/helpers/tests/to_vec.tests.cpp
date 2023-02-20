// gtest
#include <gtest/gtest.h>

// kengine glm
#include "kengine/glm/helpers/to_vec.hpp"

TEST(glm, to_vec) {
	const auto vec = kengine::glm::to_vec(putils::point3f{ 42.f, -42.f, 0.f });
	const glm::vec3 expected = { 42.f, -42.f, 0.f };
	EXPECT_EQ(vec, expected);
}