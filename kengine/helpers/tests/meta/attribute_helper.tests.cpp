// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine data
#include "kengine/data/transform.hpp"

// kengine helpers
#include "kengine/helpers/meta/attribute_helper.hpp"
#include "kengine/helpers/meta/impl/register_attributes.hpp"
#include "kengine/helpers/type_helper.hpp"

TEST(attribute_helper, find_attribute) {
	entt::registry r;
	kengine::register_attributes<kengine::data::transform>(r);

	const auto e = kengine::type_helper::get_type_entity<kengine::data::transform>(r);

	const auto attr = kengine::meta::attribute_helper::find_attribute({ r, e }, "bounding_box");
	EXPECT_NE(attr, nullptr);

	const auto expected = putils::reflection::runtime::find_attribute<kengine::data::transform>("bounding_box");
	EXPECT_EQ(attr, expected);
}