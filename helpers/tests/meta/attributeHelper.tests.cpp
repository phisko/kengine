#include "tests/KengineTest.hpp"
#include "helpers/meta/attributeHelper.hpp"
#include "helpers/registerTypeHelper.hpp"
#include "helpers/typeHelper.hpp"
#include "data/TransformComponent.hpp"

struct attributeHelper : KengineTest {};

TEST_F(attributeHelper, findAttribute) {
    kengine::registerComponents<kengine::TransformComponent>();
    const auto e = kengine::typeHelper::getTypeEntity<kengine::TransformComponent>();

    const auto attr = kengine::meta::attributeHelper::findAttribute(e, "boundingBox");
    EXPECT_NE(attr, nullptr);

    const auto expected = putils::reflection::runtime::findAttribute<kengine::TransformComponent>("boundingBox");
    EXPECT_EQ(attr, expected);
}