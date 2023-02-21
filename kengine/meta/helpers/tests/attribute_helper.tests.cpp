// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine core
#include "kengine/core/data/transform.hpp"

// kengine meta
#include "kengine/meta/helpers/attribute_helper.hpp"
#include "kengine/meta/helpers/register_metadata.hpp"
#include "kengine/meta/helpers/type_helper.hpp"

TEST(attribute_helper, find_attribute) {
	entt::registry r;
	kengine::register_metadata<kengine::core::transform>(r);

	const auto e = kengine::type_helper::get_type_entity<kengine::core::transform>(r);

	const auto attr = kengine::meta::attribute_helper::find_attribute({ r, e }, "bounding_box");
	EXPECT_NE(attr, nullptr);

	const auto expected = putils::reflection::runtime::find_attribute<kengine::core::transform>("bounding_box");
	EXPECT_EQ(attr, expected);
}