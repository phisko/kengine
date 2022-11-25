// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine data
#include "kengine/data/window.hpp"
#include "kengine/data/viewport.hpp"

// kengine helpers
#include "kengine/helpers/camera_helper.hpp"

struct params {
    kengine::data::viewport viewport_component;
    putils::point2ui pixel;
};

struct result {
    entt::entity window;
    kengine::camera_helper::viewport_info info;
};

static result get_viewport_for_pixel(const params & params) noexcept {
	entt::registry r;

	const auto e = r.create();
	r.emplace<kengine::data::window>(e);

	auto & viewport = r.emplace<kengine::data::viewport>(e);
	viewport = params.viewport_component;
	viewport.window = e;

    return { e, kengine::camera_helper::get_viewport_for_pixel({ r, e }, params.pixel) };
}

TEST(camera_helper, get_viewport_for_pixel_zro) {
    const auto result = get_viewport_for_pixel({
        .pixel = { 0, 0 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, 0.f);
    EXPECT_EQ(result.info.pixel.y, 0.f);
    EXPECT_EQ(result.info.viewport_percent.x, 0.f);
    EXPECT_EQ(result.info.viewport_percent.y, 0.f);
}

TEST(camera_helper, get_viewport_for_pixel_outside) {
    const auto result = get_viewport_for_pixel({
        .pixel = { 1280, 720 }
    });
    EXPECT_EQ(result.info.camera, entt::entity(entt::null));
    EXPECT_EQ(result.info.pixel.x, -1.f);
    EXPECT_EQ(result.info.pixel.y, -1.f);
    EXPECT_EQ(result.info.viewport_percent.x, -1.f);
    EXPECT_EQ(result.info.viewport_percent.y, -1.f);
}

TEST(camera_helper, get_viewport_for_pixel_inside) {
    const auto result = get_viewport_for_pixel({
        .pixel = { 640, 360 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .5f);
    EXPECT_EQ(result.info.pixel.y, .5f);
    EXPECT_EQ(result.info.viewport_percent.x, .5f);
    EXPECT_EQ(result.info.viewport_percent.y, .5f);
}

TEST(camera_helper, get_viewport_for_pixel_half_size_zero) {
    kengine::data::viewport viewport;
    viewport.bounding_box.size = { .5f, .5f };
    const auto result = get_viewport_for_pixel({
        .viewport_component = viewport,
        .pixel = { 0, 0 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .0f);
    EXPECT_EQ(result.info.pixel.y, .0f);
    EXPECT_EQ(result.info.viewport_percent.x, .0f);
    EXPECT_EQ(result.info.viewport_percent.y, .0f);
}

TEST(camera_helper, get_viewport_for_pixel_half_size_outside) {
    kengine::data::viewport viewport;
    viewport.bounding_box.size = { .5f, .5f };
    const auto result = get_viewport_for_pixel({
        .viewport_component = viewport,
        .pixel = { 640, 360 }
    });
    EXPECT_EQ(result.info.camera, entt::entity(entt::null));
    EXPECT_EQ(result.info.pixel.x, -1.f);
    EXPECT_EQ(result.info.pixel.y, -1.f);
    EXPECT_EQ(result.info.viewport_percent.x, -1.f);
    EXPECT_EQ(result.info.viewport_percent.y, -1.f);
}

TEST(camera_helper, get_viewport_for_pixel_half_size_inside) {
    kengine::data::viewport viewport;
    viewport.bounding_box.size = { .5f, .5f };
    const auto result = get_viewport_for_pixel({
        .viewport_component = viewport,
        .pixel = { 320, 180 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .25f);
    EXPECT_EQ(result.info.pixel.y, .25f);
    EXPECT_EQ(result.info.viewport_percent.x, .5f);
    EXPECT_EQ(result.info.viewport_percent.y, .5f);
}

TEST(camera_helper, get_viewport_for_pixel_offset_zero) {
    kengine::data::viewport viewport;
    viewport.bounding_box.position = { .5f, .5f };
    const auto result = get_viewport_for_pixel({
        .viewport_component = viewport,
        .pixel = { 640, 360 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .5f);
    EXPECT_EQ(result.info.pixel.y, .5f);
    EXPECT_EQ(result.info.viewport_percent.x, 0.f);
    EXPECT_EQ(result.info.viewport_percent.y, 0.f);
}

TEST(camera_helper, get_viewport_for_pixel_offset_outside) {
    kengine::data::viewport viewport;
    viewport.bounding_box.position = { .5f, .5f };
    const auto result = get_viewport_for_pixel({
        .viewport_component = viewport,
        .pixel = { 0, 0 }
    });
    EXPECT_EQ(result.info.camera, entt::entity(entt::null));
    EXPECT_EQ(result.info.pixel.x, -1.f);
    EXPECT_EQ(result.info.pixel.y, -1.f);
    EXPECT_EQ(result.info.viewport_percent.x, -1.f);
    EXPECT_EQ(result.info.viewport_percent.y, -1.f);
}

TEST(camera_helper, get_viewport_for_pixel_offset_inside) {
    kengine::data::viewport viewport;
    viewport.bounding_box.position = { .5f, .5f };
    const auto result = get_viewport_for_pixel({
        .viewport_component = viewport,
        .pixel = { 960, 540 }
    });
    EXPECT_EQ(result.info.camera, result.window);
    EXPECT_EQ(result.info.pixel.x, .75f);
    EXPECT_EQ(result.info.pixel.y, .75f);
    EXPECT_EQ(result.info.viewport_percent.x, .25f);
    EXPECT_EQ(result.info.viewport_percent.y, .25f);
}

TEST(camera_helper, convert_to_screen_percentage_percentage) {
    kengine::data::on_screen comp;
    comp.coordinates = kengine::data::on_screen::coordinate_type::screen_percentage;
    const putils::rect2f rect{ { .25f, .25f }, { .5f, .5f } };
    EXPECT_EQ(kengine::camera_helper::convert_to_screen_percentage(rect, { 1280, 720 }, comp), rect);
}

TEST(camera_helper, convert_to_screen_percentage_pixels) {
    kengine::data::on_screen comp;
    comp.coordinates = kengine::data::on_screen::coordinate_type::pixels;

    const putils::rect2f rect{ { 0.f, 0.f }, { 640.f, 360.f } };
    const auto screen_percentage = kengine::camera_helper::convert_to_screen_percentage(rect, { 1280, 720 }, comp);

    const auto expected_pos = putils::point2f{ 0.f, 0.f };
    EXPECT_EQ(screen_percentage.position, expected_pos);

    const auto expected_size = putils::point2f{ .5f, .5f };
    EXPECT_EQ(screen_percentage.size, expected_size);
}

TEST(camera_helper, get_facings_default) {
    const kengine::data::camera camera;
    const auto facings = kengine::camera_helper::get_facings(camera);

    const auto expected_front = putils::vec3f{ 0.f, 0.f, 1.f };
    EXPECT_EQ(facings.front, expected_front);

    // right handed I guess
    const auto expected_right = putils::vec3f{ -1.f, 0.f, 0.f };
    EXPECT_EQ(facings.right, expected_right);

    const auto expected_up = putils::vec3f{ 0.f, 1.f, 0.f };
    EXPECT_EQ(facings.up, expected_up);
}