// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/skeleton/helpers/get_bone_matrix.hpp"
#include "kengine/skeleton/helpers/set_bone_matrix.hpp"

TEST(skeleton_helper, set_bone_matrix) {
	const glm::mat4 expected{ .5f };
	const auto bone_name = "bone";

	kengine::skeleton::bone_matrices matrices{
		.meshes = { {} }
	};

	const kengine::skeleton::bone_names names{
		.meshes = { { { bone_name } } }
	};

	const entt::registry r;
	kengine::skeleton::set_bone_matrix(r, bone_name, expected, matrices, names);

	const auto mat = kengine::skeleton::get_bone_matrix(r, bone_name, matrices, names);
	EXPECT_EQ(mat, expected);
}