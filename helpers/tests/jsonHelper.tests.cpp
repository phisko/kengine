#include "tests/KengineTest.hpp"

// kengine data
#include "data/NameComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine helpers
#include "helpers/jsonHelper.hpp"
#include "helpers/registerTypeHelper.hpp"

struct jsonHelper : KengineTest {
    jsonHelper() noexcept {
        kengine::registerComponents<kengine::NameComponent, kengine::TransformComponent>();
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

TEST_F(jsonHelper, createEntity) {
    const auto e = kengine::jsonHelper::createEntity(json);

    EXPECT_TRUE(e.has<kengine::NameComponent>());
    EXPECT_EQ(e.get<kengine::NameComponent>().name, "hello");

    EXPECT_TRUE(e.has<kengine::TransformComponent>());
    const auto & transform = e.get<kengine::TransformComponent>();
    const auto & boundingBox = transform.boundingBox;
    const auto & pos = boundingBox.position;
    EXPECT_EQ(pos, expectedPos);
    const auto & size = boundingBox.size;
    EXPECT_EQ(size, expectedSize);
}

TEST_F(jsonHelper, loadEntity) {
    const auto e = kengine::entities += [this](kengine::Entity & e) {
        kengine::jsonHelper::loadEntity(json, e);
    };

    EXPECT_TRUE(e.has<kengine::NameComponent>());
    EXPECT_EQ(e.get<kengine::NameComponent>().name, "hello");

    EXPECT_TRUE(e.has<kengine::TransformComponent>());
    const auto & transform = e.get<kengine::TransformComponent>();
    const auto & boundingBox = transform.boundingBox;
    const auto & pos = boundingBox.position;
    EXPECT_EQ(pos, expectedPos);
    const auto & size = boundingBox.size;
    EXPECT_EQ(size, expectedSize);
}

TEST_F(jsonHelper, saveEntity) {
    const auto e = kengine::entities += [this](kengine::Entity & e) {
        e += kengine::NameComponent{ expectedName };
        e += kengine::TransformComponent{
            .boundingBox = {
                .position = expectedPos,
                .size = expectedSize
            }
        };
    };

    const auto json = kengine::jsonHelper::saveEntity(e);

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