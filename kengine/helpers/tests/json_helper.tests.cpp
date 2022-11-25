// entt
#include <entt/entity/handle.hpp>

// gtest
#include <gtest/gtest.h>

// kengine data
#include "kengine/data/name.hpp"
#include "kengine/data/transform.hpp"

// kengine helpers
#include "kengine/helpers/json_helper.hpp"
#include "kengine/helpers/register_type_helper.hpp"

struct json_helper : ::testing::Test {
	entt::registry r;

    json_helper() noexcept {
        kengine::register_components<kengine::data::name, kengine::data::transform>(r);
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

    const char * expected_name = "hello";
    const putils::point3f expected_pos = { 42.f, -42.f, 84.f };
    const putils::point3f expected_size = { 1.f, 2.f, 3.f };
    nlohmann::json json;
};

TEST_F(json_helper, load_entity) {
	const auto e = r.create();
	kengine::json_helper::load_entity(json, { r, e });

    EXPECT_TRUE(r.all_of<kengine::data::name>(e));
    EXPECT_EQ(r.get<kengine::data::name>(e).name, "hello");

    EXPECT_TRUE(r.all_of<kengine::data::transform>(e));
    const auto & transform = r.get<kengine::data::transform>(e);
    const auto & bounding_box = transform.bounding_box;
    const auto & pos = bounding_box.position;
    EXPECT_EQ(pos, expected_pos);
    const auto & size = bounding_box.size;
    EXPECT_EQ(size, expected_size);
}

TEST_F(json_helper, save_entity) {
	const auto e = r.create();
	r.emplace<kengine::data::name>(e, expected_name);
	r.emplace<kengine::data::transform>(e) = {
		.bounding_box = {
			.position = expected_pos,
			.size = expected_size
		}
	};

    const auto json = kengine::json_helper::save_entity({ r, e });

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