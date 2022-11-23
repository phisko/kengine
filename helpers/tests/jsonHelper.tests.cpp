// entt
#include <entt/entity/handle.hpp>

// gtest
#include <gtest/gtest.h>

// kengine data
#include "data/NameComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine helpers
#include "helpers/jsonHelper.hpp"
#include "helpers/registerTypeHelper.hpp"

struct jsonHelper : ::testing::Test {
	entt::registry r;

    jsonHelper() noexcept {
        kengine::registerComponents<kengine::NameComponent, kengine::TransformComponent>(r);
        json["NameComponent"]["name"] = expectedName;
        auto &transform = json["TransformComponent"];
        auto &boundingBox = transform["boundingBox"];
        auto &pos = boundingBox["position"];
        pos["x"] = expectedPos.x;
        pos["y"] = expectedPos.y;
        pos["z"] = expectedPos.z;
        auto &size = boundingBox["size"];
        size["x"] = expectedSize.x;
        size["y"] = expectedSize.y;
        size["z"] = expectedSize.z;
    }

    const char * expectedName = "hello";
    const putils::Point3f expectedPos = { 42.f, -42.f, 84.f };
    const putils::Point3f expectedSize = { 1.f, 2.f, 3.f };
    nlohmann::json json;
};

TEST_F(jsonHelper, loadEntity) {
	const auto e = r.create();
	kengine::jsonHelper::loadEntity(json, { r, e });

    EXPECT_TRUE(r.all_of<kengine::NameComponent>(e));
    EXPECT_EQ(r.get<kengine::NameComponent>(e).name, "hello");

    EXPECT_TRUE(r.all_of<kengine::TransformComponent>(e));
    const auto & transform = r.get<kengine::TransformComponent>(e);
    const auto & boundingBox = transform.boundingBox;
    const auto & pos = boundingBox.position;
    EXPECT_EQ(pos, expectedPos);
    const auto & size = boundingBox.size;
    EXPECT_EQ(size, expectedSize);
}

TEST_F(jsonHelper, saveEntity) {
	const auto e = r.create();
	r.emplace<kengine::NameComponent>(e, expectedName);
	r.emplace<kengine::TransformComponent>(e) = {
		.boundingBox = {
			.position = expectedPos,
			.size = expectedSize
		}
	};

    const auto json = kengine::jsonHelper::saveEntity({ r, e });

    EXPECT_EQ(json["NameComponent"]["name"], expectedName);

    const auto & transform = json["TransformComponent"];
    const auto & boundingBox = transform["boundingBox"];
    const auto & pos = boundingBox["position"];
    EXPECT_EQ(pos["x"].get<float>(), expectedPos.x);
    EXPECT_EQ(pos["y"].get<float>(), expectedPos.y);
    EXPECT_EQ(pos["z"].get<float>(), expectedPos.z);
    const auto & size = boundingBox["size"];
    EXPECT_EQ(size["x"].get<float>(), expectedSize.x);
    EXPECT_EQ(size["y"].get<float>(), expectedSize.y);
    EXPECT_EQ(size["z"].get<float>(), expectedSize.z);
}