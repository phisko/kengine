// entt
#include <entt/entity/handle.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/meta/helpers/register_metadata.hpp"
#include "kengine/meta/helpers/register_meta_component_implementation.hpp"
#include "kengine/meta/json/functions/load.hpp"
#include "kengine/meta/json/helpers/impl/load.hpp"
#include "kengine/meta/json/helpers/load_entity.hpp"

TEST(meta_json, load_entity) {
	entt::registry r;
	kengine::meta::register_metadata<kengine::core::name, kengine::core::transform>(r);
	kengine::meta::register_meta_component_implementation<
		kengine::meta::json::load,
		kengine::core::name, kengine::core::transform
	>(r);

	const char * expected_name = "hello";
	const putils::point3f expected_pos = { 42.f, -42.f, 84.f };
	const putils::point3f expected_size = { 1.f, 2.f, 3.f };

	nlohmann::json json;
	{
		json["name"]["name"] = expected_name;
		auto & transform = json["transform"];
		auto & bounding_box = transform["bounding_box"];
		auto & pos = bounding_box["position"];
		pos["x"] = expected_pos.x;
		pos["y"] = expected_pos.y;
		pos["z"] = expected_pos.z;
		auto & size = bounding_box["size"];
		size["x"] = expected_size.x;
		size["y"] = expected_size.y;
		size["z"] = expected_size.z;
	}

	const auto e = r.create();
	kengine::meta::json::load_entity(json, { r, e });

	EXPECT_TRUE(r.all_of<kengine::core::name>(e));
	EXPECT_EQ(r.get<kengine::core::name>(e).name, expected_name);

	EXPECT_TRUE(r.all_of<kengine::core::transform>(e));
	const auto & transform = r.get<kengine::core::transform>(e);
	const auto & bounding_box = transform.bounding_box;
	const auto & pos = bounding_box.position;
	EXPECT_EQ(pos, expected_pos);
	const auto & size = bounding_box.size;
	EXPECT_EQ(size, expected_size);
}