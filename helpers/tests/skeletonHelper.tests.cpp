#ifdef KENGINE_GLM

#include "tests/KengineTest.hpp"

// kengine helpers
#include "helpers/skeletonHelper.hpp"

struct skeletonHelper : KengineTest {};

TEST_F(skeletonHelper, getBoneIndex) {
    const kengine::ModelSkeletonComponent comp {
        .meshes = {
            { { "0.0", "0.1" } },
            { { "1.0", "1.1" } },
        }
    };

    auto index = kengine::skeletonHelper::getBoneIndex("0.0", comp);
    EXPECT_EQ(index.meshIndex, 0);
    EXPECT_EQ(index.boneIndex, 0);

    index = kengine::skeletonHelper::getBoneIndex("0.1", comp);
    EXPECT_EQ(index.meshIndex, 0);
    EXPECT_EQ(index.boneIndex, 1);

    index = kengine::skeletonHelper::getBoneIndex("1.0", comp);
    EXPECT_EQ(index.meshIndex, 1);
    EXPECT_EQ(index.boneIndex, 0);

    index = kengine::skeletonHelper::getBoneIndex("1.1", comp);
    EXPECT_EQ(index.meshIndex, 1);
    EXPECT_EQ(index.boneIndex, 1);
}

TEST_F(skeletonHelper, getBoneMatrix) {
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

    const auto mat = kengine::skeletonHelper::getBoneMatrix(boneName, comp, model);
    EXPECT_EQ(mat, expected);
}

TEST_F(skeletonHelper, setBoneMatrix) {
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

    kengine::skeletonHelper::setBoneMatrix(boneName, expected, comp, model);

    const auto mat = kengine::skeletonHelper::getBoneMatrix(boneName, comp, model);
    EXPECT_EQ(mat, expected);
}

#endif