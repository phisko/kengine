// gtest
#include <gtest/gtest.h>

// glm
#include <glm/ext/matrix_transform.hpp>

// kengine glm
#include "kengine/glm/helpers/convert_to_referencial.hpp"

TEST(glm, convert_to_referencial) {
	::glm::mat4 mat{ 1.f };
	mat = ::glm::translate(mat, { -1.f, 0.f, 0.f });
	const auto pos = kengine::glm::convert_to_referencial({ 0.f, 0.f, 0.f }, mat);
	const putils::point3f expected{ -1.f, 0.f, 0.f };
	EXPECT_EQ(pos, expected);
}