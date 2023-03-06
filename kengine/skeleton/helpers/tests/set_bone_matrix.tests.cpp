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

	kengine::skeleton::skeleton comp{
		.meshes = { {} }
	};

	const kengine::skeleton::model_skeleton model{
		.meshes = { { { bone_name } } }
	};

	const entt::registry r;
	kengine::skeleton::set_bone_matrix(r, bone_name, expected, comp, model);

	const auto mat = kengine::skeleton::get_bone_matrix(r, bone_name, comp, model);
	EXPECT_EQ(mat, expected);
}