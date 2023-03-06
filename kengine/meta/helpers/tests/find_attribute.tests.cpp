// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/meta/helpers/find_attribute.hpp"
#include "kengine/meta/helpers/get_type_entity.hpp"
#include "kengine/meta/helpers/register_metadata.hpp"

TEST(meta, find_attribute) {
	entt::registry r;
	kengine::meta::register_metadata<kengine::core::transform>(r);

	const auto e = kengine::meta::get_type_entity<kengine::core::transform>(r);

	const auto attr = kengine::meta::find_attribute({ r, e }, "bounding_box");
	EXPECT_NE(attr, nullptr);

	const auto expected = putils::reflection::runtime::find_attribute<kengine::core::transform>("bounding_box");
	EXPECT_EQ(attr, expected);
}