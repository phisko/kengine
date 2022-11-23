#ifdef KENGINE_GLM

// gtest
#include <gtest/gtest.h>

// kengine data
#include "data/TransformComponent.hpp"

// kengine helpers
#include "helpers/matrixHelper.hpp"

TEST(matrixHelper, toVec) {
    const auto vec = kengine::matrixHelper::toVec(putils::Point3f{ 42.f, -42.f, 0.f });
    const glm::vec3 expected = { 42.f, -42.f, 0.f };
    EXPECT_EQ(vec, expected);
}

TEST(matrixHelper, getPosition) {
    glm::mat4 mat{ 1.f };
    mat = glm::translate(mat, { 42.f, -42.f, 0.f });
    const auto pos = kengine::matrixHelper::getPosition(mat);
    const putils::Point3f expected{ 42.f, -42.f, 0.f };
    EXPECT_EQ(pos, expected);
}

TEST(matrixHelper, getScale) {
    glm::mat4 mat{ 1.f };
    mat = glm::scale(mat, { 42.f, -42.f, 1.f });
    const auto scale = kengine::matrixHelper::getScale(mat);
    const putils::Vector3f expected{ 42.f, 42.f, 1.f };
    EXPECT_EQ(scale, expected);
}

TEST(matrixHelper, getRotation) {
    glm::mat4 mat{ 1.f };
    mat = glm::rotate(mat, .5f, { 0.f, 1.f, 0.f });
    const auto rotation = kengine::matrixHelper::getRotation(mat);
    const putils::Vector3f expected{ 0.f, .5f, 0.f };
    EXPECT_EQ(rotation, expected);
}

TEST(matrixHelper, convertToReferencial) {
    glm::mat4 mat{ 1.f };
    mat = glm::translate(mat, { -1.f, 0.f, 0.f });
    const auto pos = kengine::matrixHelper::convertToReferencial({ 0.f, 0.f, 0.f }, mat);
    const putils::Point3f expected{ -1.f, 0.f, 0.f };
    EXPECT_EQ(pos, expected);
}

TEST(matrixHelper, getModelMatrix) {
    const putils::Point3f expectedPos{ 42.f, -42.f, 0.f };
    const putils::Vector3f expectedScale{ 42.f, 42.f, 1.f };

    const kengine::TransformComponent transform{
        .boundingBox = {
            .position = expectedPos,
            .size = expectedScale
        }
    };

    const auto mat = kengine::matrixHelper::getModelMatrix(transform);

    const auto pos = kengine::matrixHelper::getPosition(mat);
    EXPECT_EQ(pos, expectedPos);

    const auto scale = kengine::matrixHelper::getScale(mat);
    EXPECT_EQ(scale, expectedScale);

    const putils::Vector3f expectedRotation{ 0.f, 0.f, 0.f };
    const auto rotation = kengine::matrixHelper::getRotation(mat);
    EXPECT_EQ(rotation, expectedRotation);
}

#endif