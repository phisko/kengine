// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/render/helpers/get_facings.hpp"

TEST(camera_helper, get_facings_default) {
	const kengine::render::camera camera;
	const auto facings = kengine::render::get_facings(camera);

	const auto expected_front = putils::vec3f{ 0.f, 0.f, 1.f };
	EXPECT_EQ(facings.front, expected_front);

	// right handed I guess
	const auto expected_right = putils::vec3f{ -1.f, 0.f, 0.f };
	EXPECT_EQ(facings.right, expected_right);

	const auto expected_up = putils::vec3f{ 0.f, 1.f, 0.f };
	EXPECT_EQ(facings.up, expected_up);
}
