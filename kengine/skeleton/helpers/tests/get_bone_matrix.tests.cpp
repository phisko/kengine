// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/skeleton/helpers/get_bone_matrix.hpp"

TEST(skeleton_helper, get_bone_matrix) {
	const glm::mat4 expected{ .5f };
	const auto bone_name = "bone";

	const kengine::skeleton::skeleton comp{
		.meshes = { { .bone_mats_mesh_space = { expected } } }
	};

	const kengine::skeleton::bone_names model{
		.meshes = { { { bone_name } } }
	};

	const entt::registry r;
	const auto mat = kengine::skeleton::get_bone_matrix(r, bone_name, comp, model);
	EXPECT_EQ(mat, expected);
}