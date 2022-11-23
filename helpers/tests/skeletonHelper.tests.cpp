#ifdef KENGINE_GLM

// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine helpers
#include "helpers/skeletonHelper.hpp"

TEST(skeletonHelper, getBoneIndex) {
    const kengine::ModelSkeletonComponent comp {
        .meshes = {
            { { "0.0", "0.1" } },
            { { "1.0", "1.1" } },
        }
    };

	const entt::registry r;

    auto index = kengine::skeletonHelper::getBoneIndex(r, "0.0", comp);
    EXPECT_EQ(index.meshIndex, 0);
    EXPECT_EQ(index.boneIndex, 0);

    index = kengine::skeletonHelper::getBoneIndex(r, "0.1", comp);
    EXPECT_EQ(index.meshIndex, 0);
    EXPECT_EQ(index.boneIndex, 1);

    index = kengine::skeletonHelper::getBoneIndex(r, "1.0", comp);
    EXPECT_EQ(index.meshIndex, 1);
    EXPECT_EQ(index.boneIndex, 0);

    index = kengine::skeletonHelper::getBoneIndex(r, "1.1", comp);
    EXPECT_EQ(index.meshIndex, 1);
    EXPECT_EQ(index.boneIndex, 1);
}

TEST(skeletonHelper, getBoneMatrix) {
    const glm::mat4 expected{ .5f };
    const auto boneName = "bone";

    const kengine::SkeletonComponent comp {
        .meshes = {
            { .boneMatsMeshSpace = { expected } }
        }
    };

    const kengine::ModelSkeletonComponent model {
        .meshes = {
            { { boneName } }
        }
    };

	const entt::registry r;
    const auto mat = kengine::skeletonHelper::getBoneMatrix(r, boneName, comp, model);
    EXPECT_EQ(mat, expected);
}

TEST(skeletonHelper, setBoneMatrix) {
    const glm::mat4 expected{ .5f };
    const auto boneName = "bone";

    kengine::SkeletonComponent comp {
        .meshes = {
            {}
        }
    };

    const kengine::ModelSkeletonComponent model {
        .meshes = {
            { { boneName } }
        }
    };

	const entt::registry r;
    kengine::skeletonHelper::setBoneMatrix(r, boneName, expected, comp, model);

    const auto mat = kengine::skeletonHelper::getBoneMatrix(r, boneName, comp, model);
    EXPECT_EQ(mat, expected);
}

#endif