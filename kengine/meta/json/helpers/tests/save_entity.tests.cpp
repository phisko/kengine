// entt
#include <entt/entity/handle.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/helpers/impl/has.hpp"
#include "kengine/meta/helpers/register_metadata.hpp"
#include "kengine/meta/helpers/register_meta_component_implementation.hpp"
#include "kengine/meta/json/functions/save.hpp"
#include "kengine/meta/json/helpers/impl/save.hpp"
#include "kengine/meta/json/helpers/save_entity.hpp"

TEST(meta_json, save_entity) {
	entt::registry r;
	kengine::meta::register_metadata<kengine::core::name, kengine::core::transform>(r);
	kengine::meta::register_meta_component_implementation<
		kengine::meta::json::save,
		kengine::core::name, kengine::core::transform
	>(r);
	kengine::meta::register_meta_component_implementation<
		kengine::meta::has,
		kengine::core::name, kengine::core::transform
	>(r);

	const char * expected_name = "hello";
	const putils::point3f expected_pos = { 42.f, -42.f, 84.f };
	const putils::point3f expected_size = { 1.f, 2.f, 3.f };

	const auto e = r.create();
	r.emplace<kengine::core::name>(e, expected_name);
	r.emplace<kengine::core::transform>(e) = {
		.bounding_box = {
			.position = expected_pos,
			.size = expected_size,
		}
	};

	const auto json = kengine::meta::json::save_entity({ r, e });

	EXPECT_EQ(json["name"]["name"], expected_name);

	const auto & transform = json["transform"];
	const auto & bounding_box = transform["bounding_box"];
	const auto & pos = bounding_box["position"];
	EXPECT_EQ(pos["x"].get<float>(), expected_pos.x);
	EXPECT_EQ(pos["y"].get<float>(), expected_pos.y);
	EXPECT_EQ(pos["z"].get<float>(), expected_pos.z);
	const auto & size = bounding_box["size"];
	EXPECT_EQ(size["x"].get<float>(), expected_size.x);
	EXPECT_EQ(size["y"].get<float>(), expected_size.y);
	EXPECT_EQ(size["z"].get<float>(), expected_size.z);
}
