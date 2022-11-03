#include "tests/KengineTest.hpp"

// kengine data
#include "data/WindowComponent.hpp"
#include "data/ViewportComponent.hpp"

// kengine helpers
#include "helpers/cameraHelper.hpp"

struct cameraHelper : KengineTest {};

struct Params {
    kengine::ViewportComponent viewportComponent;
    putils::Point2ui pixel;
};

struct Result {
    kengine::EntityID window;
    kengine::cameraHelper::ViewportInfo info;
};

static Result getViewportForPixel(const Params & params) noexcept {
    kengine::EntityID id;
    kengine::entities += [&](kengine::Entity & e) {
        id = e.id;

        e += kengine::WindowComponent{};
        auto & viewport = e.attach<kengine::ViewportComponent>();
        viewport = params.viewportComponent;
        viewport.window = id;
    };

    return { id, kengine::cameraHelper::getViewportForPixel(id, params.pixel) };
}

TEST_F(cameraHelper, getViewportForPixelZero) {
    const auto result = getViewportForPixel({
        .pixel = { 0, 0 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, 0.f);
    EXPECT_EQ(result.info.pixel.y, 0.f);
    EXPECT_EQ(result.info.viewportPercent.x, 0.f);
    EXPECT_EQ(result.info.viewportPercent.y, 0.f);
}

TEST_F(cameraHelper, getViewportForPixelOutside) {
    const auto result = getViewportForPixel({
        .pixel = { 1280, 720 }
    });
    EXPECT_EQ(result.info.camera, kengine::INVALID_ID);
    EXPECT_EQ(result.info.pixel.x, -1.f);
    EXPECT_EQ(result.info.pixel.y, -1.f);
    EXPECT_EQ(result.info.viewportPercent.x, -1.f);
    EXPECT_EQ(result.info.viewportPercent.y, -1.f);
}

TEST_F(cameraHelper, getViewportForPixelInside) {
    const auto result = getViewportForPixel({
        .pixel = { 640, 360 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .5f);
    EXPECT_EQ(result.info.pixel.y, .5f);
    EXPECT_EQ(result.info.viewportPercent.x, .5f);
    EXPECT_EQ(result.info.viewportPercent.y, .5f);
}

TEST_F(cameraHelper, getViewportForPixelHalfSizeZero) {
    kengine::ViewportComponent viewport;
    viewport.boundingBox.size = { .5f, .5f };
    const auto result = getViewportForPixel({
        .viewportComponent = viewport,
        .pixel = { 0, 0 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .0f);
    EXPECT_EQ(result.info.pixel.y, .0f);
    EXPECT_EQ(result.info.viewportPercent.x, .0f);
    EXPECT_EQ(result.info.viewportPercent.y, .0f);
}

TEST_F(cameraHelper, getViewportForPixelHalfSizeOutside) {
    kengine::ViewportComponent viewport;
    viewport.boundingBox.size = { .5f, .5f };
    const auto result = getViewportForPixel({
        .viewportComponent = viewport,
        .pixel = { 640, 360 }
    });
    EXPECT_EQ(result.info.camera, kengine::INVALID_ID);
    EXPECT_EQ(result.info.pixel.x, -1.f);
    EXPECT_EQ(result.info.pixel.y, -1.f);
    EXPECT_EQ(result.info.viewportPercent.x, -1.f);
    EXPECT_EQ(result.info.viewportPercent.y, -1.f);
}

TEST_F(cameraHelper, getViewportForPixelHalfSizeInside) {
    kengine::ViewportComponent viewport;
    viewport.boundingBox.size = { .5f, .5f };
    const auto result = getViewportForPixel({
        .viewportComponent = viewport,
        .pixel = { 320, 180 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .25f);
    EXPECT_EQ(result.info.pixel.y, .25f);
    EXPECT_EQ(result.info.viewportPercent.x, .5f);
    EXPECT_EQ(result.info.viewportPercent.y, .5f);
}

TEST_F(cameraHelper, getViewportForPixelOffsetZero) {
    kengine::ViewportComponent viewport;
    viewport.boundingBox.position = { .5f, .5f };
    const auto result = getViewportForPixel({
        .viewportComponent = viewport,
        .pixel = { 640, 360 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .5f);
    EXPECT_EQ(result.info.pixel.y, .5f);
    EXPECT_EQ(result.info.viewportPercent.x, 0.f);
    EXPECT_EQ(result.info.viewportPercent.y, 0.f);
}

TEST_F(cameraHelper, getViewportForPixelOffsetOutside) {
    kengine::ViewportComponent viewport;
    viewport.boundingBox.position = { .5f, .5f };
    const auto result = getViewportForPixel({
        .viewportComponent = viewport,
        .pixel = { 0, 0 }
    });
    EXPECT_EQ(result.info.camera, kengine::INVALID_ID);
    EXPECT_EQ(result.info.pixel.x, -1.f);
    EXPECT_EQ(result.info.pixel.y, -1.f);
    EXPECT_EQ(result.info.viewportPercent.x, -1.f);
    EXPECT_EQ(result.info.viewportPercent.y, -1.f);
}

TEST_F(cameraHelper, getViewportForPixelOffsetInside) {
    kengine::ViewportComponent viewport;
    viewport.boundingBox.position = { .5f, .5f };
    const auto result = getViewportForPixel({
        .viewportComponent = viewport,
        .pixel = { 960, 540 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .75f);
    EXPECT_EQ(result.info.pixel.y, .75f);
    EXPECT_EQ(result.info.viewportPercent.x, .25f);
    EXPECT_EQ(result.info.viewportPercent.y, .25f);
}

TEST_F(cameraHelper, convertToScreenPercentage_Percentage) {
    kengine::OnScreenComponent comp;
    comp.coordinateType = kengine::OnScreenComponent::CoordinateType::ScreenPercentage;
    const putils::Rect2f rect{ { .25f, .25f }, { .5f, .5f } };
    EXPECT_EQ(kengine::cameraHelper::convertToScreenPercentage(rect, { 1280, 720 }, comp), rect);
}

TEST_F(cameraHelper, convertToScreenPercentage_Pixels) {
    kengine::OnScreenComponent comp;
    comp.coordinateType = kengine::OnScreenComponent::CoordinateType::Pixels;

    const putils::Rect2f rect{ { 0.f, 0.f }, { 640.f, 360.f } };
    const auto screenPercentage = kengine::cameraHelper::convertToScreenPercentage(rect, { 1280, 720 }, comp);

    const auto expectedPos = putils::Point2f{ 0.f, 0.f };
    EXPECT_EQ(screenPercentage.position, expectedPos);

    const auto expectedSize = putils::Point2f{ .5f, .5f };
    EXPECT_EQ(screenPercentage.size, expectedSize);
}

TEST_F(cameraHelper, getFacings_Default) {
    const kengine::CameraComponent camera;
    const auto facings = kengine::cameraHelper::getFacings(camera);

    const auto expectedFront = putils::Vector3f{ 0.f, 0.f, 1.f };
    EXPECT_EQ(facings.front, expectedFront);

    // right handed I guess
    const auto expectedRight = putils::Vector3f{ -1.f, 0.f, 0.f };
    EXPECT_EQ(facings.right, expectedRight);

    const auto expectedUp = putils::Vector3f{ 0.f, 1.f, 0.f };
    EXPECT_EQ(facings.up, expectedUp);
}