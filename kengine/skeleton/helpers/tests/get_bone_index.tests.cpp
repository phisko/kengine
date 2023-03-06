// entt
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/skeleton/helpers/get_bone_index.hpp"

TEST(skeleton_helper, get_bone_index) {
	const kengine::skeleton::bone_names names{
		.meshes = {
			{ { "0.0", "0.1" } },
			{ { "1.0", "1.1" } },
		}
	};

	const entt::registry r;

	auto index = kengine::skeleton::get_bone_index(r, "0.0", names);
	EXPECT_EQ(index.mesh_index, 0);
	EXPECT_EQ(index.bone_index, 0);

	index = kengine::skeleton::get_bone_index(r, "0.1", names);
	EXPECT_EQ(index.mesh_index, 0);
	EXPECT_EQ(index.bone_index, 1);

	index = kengine::skeleton::get_bone_index(r, "1.0", names);
	EXPECT_EQ(index.mesh_index, 1);
	EXPECT_EQ(index.bone_index, 0);

	index = kengine::skeleton::get_bone_index(r, "1.1", names);
	EXPECT_EQ(index.mesh_index, 1);
	EXPECT_EQ(index.bone_index, 1);
}