#include "tests/KengineTest.hpp"

// kengine data
#include "kengine/data/transform.hpp"

// kengine helpers
#include "kengine/helpers/meta/attribute_helper.hpp"
#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/helpers/type_helper.hpp"

struct attribute_helper : KengineTest {};

TEST_F(attribute_helper, find_attribute) {
	kengine::register_components<kengine::data::transform>();
	const auto e = kengine::type_helper::get_type_entity<kengine::data::transform>();

	const auto attr = kengine::meta::attribute_helper::find_attribute(e, "bounding_box");
	EXPECT_NE(attr, nullptr);

	const auto expected = putils::reflection::runtime::find_attribute<kengine::data::transform>("bounding_box");
	EXPECT_EQ(attr, expected);
}