#ifdef KENGINE_GLM

// gtest
#include <gtest/gtest.h>

// kengine data
#include "kengine/data/transform.hpp"

// kengine helpers
#include "kengine/helpers/matrix_helper.hpp"

TEST(matrix_helper, to_vec) {
    const auto vec = kengine::matrix_helper::to_vec(putils::point3f{ 42.f, -42.f, 0.f });
    const glm::vec3 expected = { 42.f, -42.f, 0.f };
    EXPECT_EQ(vec, expected);
}

TEST(matrix_helper, get_position) {
    glm::mat4 mat{ 1.f };
    mat = glm::translate(mat, { 42.f, -42.f, 0.f });
    const auto pos = kengine::matrix_helper::get_position(mat);
    const putils::point3f expected{ 42.f, -42.f, 0.f };
    EXPECT_EQ(pos, expected);
}

TEST(matrix_helper, get_scale) {
    glm::mat4 mat{ 1.f };
    mat = glm::scale(mat, { 42.f, -42.f, 1.f });
    const auto scale = kengine::matrix_helper::get_scale(mat);
    const putils::vec3f expected{ 42.f, 42.f, 1.f };
    EXPECT_EQ(scale, expected);
}

TEST(matrix_helper, get_rotation) {
    glm::mat4 mat{ 1.f };
    mat = glm::rotate(mat, .5f, { 0.f, 1.f, 0.f });
    const auto rotation = kengine::matrix_helper::get_rotation(mat);
    const putils::vec3f expected{ 0.f, .5f, 0.f };
    EXPECT_EQ(rotation, expected);
}

TEST(matrix_helper, convert_to_referencial) {
    glm::mat4 mat{ 1.f };
    mat = glm::translate(mat, { -1.f, 0.f, 0.f });
    const auto pos = kengine::matrix_helper::convert_to_referencial({ 0.f, 0.f, 0.f }, mat);
    const putils::point3f expected{ -1.f, 0.f, 0.f };
    EXPECT_EQ(pos, expected);
}

TEST(matrix_helper, get_model_matrix) {
    const putils::point3f expected_pos{ 42.f, -42.f, 0.f };
    const putils::vec3f expected_scale{ 42.f, 42.f, 1.f };

    const kengine::data::transform transform{
        .bounding_box = {
            .position = expected_pos,
            .size = expected_scale
        }
    };

    const auto mat = kengine::matrix_helper::get_model_matrix(transform);

    const auto pos = kengine::matrix_helper::get_position(mat);
    EXPECT_EQ(pos, expected_pos);

    const auto scale = kengine::matrix_helper::get_scale(mat);
    EXPECT_EQ(scale, expected_scale);

    const putils::vec3f expected_rotation{ 0.f, 0.f, 0.f };
    const auto rotation = kengine::matrix_helper::get_rotation(mat);
    EXPECT_EQ(rotation, expected_rotation);
}

#endif