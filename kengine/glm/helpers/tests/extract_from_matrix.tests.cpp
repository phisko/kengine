// gtest
#include <gtest/gtest.h>

// glm
#include <glm/ext/matrix_transform.hpp>

// kengine glm
#include "kengine/glm/helpers/extract_from_matrix.hpp"

TEST(glm, extract_position) {
	glm::mat4 mat{ 1.f };
	mat = glm::translate(mat, { 42.f, -42.f, 0.f });
	const auto pos = kengine::glm::extract_position(mat);
	const putils::point3f expected{ 42.f, -42.f, 0.f };
	EXPECT_EQ(pos, expected);
}

TEST(glm, extract_scale) {
	glm::mat4 mat{ 1.f };
	mat = glm::scale(mat, { 42.f, -42.f, 1.f });
	const auto scale = kengine::glm::extract_scale(mat);
	const putils::vec3f expected{ 42.f, 42.f, 1.f };
	EXPECT_EQ(scale, expected);
}

TEST(glm, extract_rotation) {
	glm::mat4 mat{ 1.f };
	mat = glm::rotate(mat, .5f, { 0.f, 1.f, 0.f });
	const auto rotation = kengine::glm::extract_rotation(mat);
	const putils::vec3f expected{ 0.f, .5f, 0.f };
	EXPECT_EQ(rotation, expected);
}