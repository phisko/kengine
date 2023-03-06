// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/glm/helpers/extract_from_matrix.hpp"
#include "kengine/glm/helpers/get_model_matrix.hpp"

TEST(glm, get_model_matrix) {
	const putils::point3f expected_pos{ 42.f, -42.f, 0.f };
	const putils::vec3f expected_scale{ 42.f, 42.f, 1.f };

	const kengine::core::transform transform{
		.bounding_box = {
			.position = expected_pos,
			.size = expected_scale,
		}
	};

	const auto mat = kengine::glm::get_model_matrix(transform);

	const auto pos = kengine::glm::extract_position(mat);
	EXPECT_EQ(pos, expected_pos);

	const auto scale = kengine::glm::extract_scale(mat);
	EXPECT_EQ(scale, expected_scale);

	const putils::vec3f expected_rotation{ 0.f, 0.f, 0.f };
	const auto rotation = kengine::glm::extract_rotation(mat);
	EXPECT_EQ(rotation, expected_rotation);
}
