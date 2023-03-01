// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine skeleton
#include "kengine/skeleton/helpers/skeleton_helper.hpp"

TEST(skeleton_helper, get_bone_index) {
	const kengine::data::model_skeleton comp{
		.meshes = {
			{ { "0.0", "0.1" } },
			{ { "1.0", "1.1" } },
		}
	};

	const entt::registry r;

	auto index = kengine::skeleton_helper::get_bone_index(r, "0.0", comp);
	EXPECT_EQ(index.mesh_index, 0);
	EXPECT_EQ(index.bone_index, 0);

	index = kengine::skeleton_helper::get_bone_index(r, "0.1", comp);
	EXPECT_EQ(index.mesh_index, 0);
	EXPECT_EQ(index.bone_index, 1);

	index = kengine::skeleton_helper::get_bone_index(r, "1.0", comp);
	EXPECT_EQ(index.mesh_index, 1);
	EXPECT_EQ(index.bone_index, 0);

	index = kengine::skeleton_helper::get_bone_index(r, "1.1", comp);
	EXPECT_EQ(index.mesh_index, 1);
	EXPECT_EQ(index.bone_index, 1);
}

TEST(skeleton_helper, get_bone_matrix) {
	const glm::mat4 expected{ .5f };
	const auto bone_name = "bone";

	const kengine::data::skeleton comp{
		.meshes = { { .bone_mats_mesh_space = { expected } } }
	};

	const kengine::data::model_skeleton model{
		.meshes = { { { bone_name } } }
	};

	const entt::registry r;
	const auto mat = kengine::skeleton_helper::get_bone_matrix(r, bone_name, comp, model);
	EXPECT_EQ(mat, expected);
}

TEST(skeleton_helper, set_bone_matrix) {
	const glm::mat4 expected{ .5f };
	const auto bone_name = "bone";

	kengine::data::skeleton comp{
		.meshes = { {} }
	};

	const kengine::data::model_skeleton model{
		.meshes = { { { bone_name } } }
	};

	const entt::registry r;
	kengine::skeleton_helper::set_bone_matrix(r, bone_name, expected, comp, model);

	const auto mat = kengine::skeleton_helper::get_bone_matrix(r, bone_name, comp, model);
	EXPECT_EQ(mat, expected);
}