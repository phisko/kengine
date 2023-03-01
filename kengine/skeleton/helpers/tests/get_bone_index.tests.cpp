// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine skeleton
#include "kengine/skeleton/helpers/get_bone_index.hpp"

TEST(skeleton_helper, get_bone_index) {
	const kengine::skeleton::model_skeleton comp{
		.meshes = {
			{ { "0.0", "0.1" } },
			{ { "1.0", "1.1" } },
		}
	};

	const entt::registry r;

	auto index = kengine::skeleton::get_bone_index(r, "0.0", comp);
	EXPECT_EQ(index.mesh_index, 0);
	EXPECT_EQ(index.bone_index, 0);

	index = kengine::skeleton::get_bone_index(r, "0.1", comp);
	EXPECT_EQ(index.mesh_index, 0);
	EXPECT_EQ(index.bone_index, 1);

	index = kengine::skeleton::get_bone_index(r, "1.0", comp);
	EXPECT_EQ(index.mesh_index, 1);
	EXPECT_EQ(index.bone_index, 0);

	index = kengine::skeleton::get_bone_index(r, "1.1", comp);
	EXPECT_EQ(index.mesh_index, 1);
	EXPECT_EQ(index.bone_index, 1);
}