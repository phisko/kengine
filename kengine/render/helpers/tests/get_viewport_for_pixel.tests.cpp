// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine render
#include "kengine/render/data/window.hpp"
#include "kengine/render/data/viewport.hpp"
#include "kengine/render/helpers/get_viewport_for_pixel.hpp"

struct params {
	kengine::render::viewport viewport_component;
	putils::point2ui pixel;
};

struct result {
	entt::entity window;
	kengine::render::viewport_info info;
};

static result get_viewport_for_pixel(const params & params) noexcept {
	entt::registry r;

	const auto e = r.create();
	r.emplace<kengine::render::window>(e);

	auto & viewport = r.emplace<kengine::render::viewport>(e);
	viewport = params.viewport_component;
	viewport.window = e;

	return { e, kengine::render::get_viewport_for_pixel({ r, e }, params.pixel) };
}

TEST(camera_helper, get_viewport_for_pixel_zro) {
	const auto result = get_viewport_for_pixel({ .pixel = { 0, 0 } });
	EXPECT_EQ(result.info.camera, result.window);
	EXPECT_EQ(result.info.pixel.x, 0.f);
	EXPECT_EQ(result.info.pixel.y, 0.f);
	EXPECT_EQ(result.info.viewport_percent.x, 0.f);
	EXPECT_EQ(result.info.viewport_percent.y, 0.f);
}

TEST(camera_helper, get_viewport_for_pixel_outside) {
	const auto result = get_viewport_for_pixel({ .pixel = { 1280, 720 } });
	EXPECT_EQ(result.info.camera, entt::entity(entt::null));
	EXPECT_EQ(result.info.pixel.x, -1.f);
	EXPECT_EQ(result.info.pixel.y, -1.f);
	EXPECT_EQ(result.info.viewport_percent.x, -1.f);
	EXPECT_EQ(result.info.viewport_percent.y, -1.f);
}

TEST(camera_helper, get_viewport_for_pixel_inside) {
	const auto result = get_viewport_for_pixel({ .pixel = { 640, 360 } });
	EXPECT_EQ(result.info.camera, result.window);
	EXPECT_EQ(result.info.pixel.x, .5f);
	EXPECT_EQ(result.info.pixel.y, .5f);
	EXPECT_EQ(result.info.viewport_percent.x, .5f);
	EXPECT_EQ(result.info.viewport_percent.y, .5f);
}

TEST(camera_helper, get_viewport_for_pixel_half_size_zero) {
	kengine::render::viewport viewport;
	viewport.bounding_box.size = { .5f, .5f };
	const auto result = get_viewport_for_pixel({
		.viewport_component = viewport,
		.pixel = { 0, 0 },
	});
	EXPECT_EQ(result.info.camera, result.window);
	EXPECT_EQ(result.info.pixel.x, .0f);
	EXPECT_EQ(result.info.pixel.y, .0f);
	EXPECT_EQ(result.info.viewport_percent.x, .0f);
	EXPECT_EQ(result.info.viewport_percent.y, .0f);
}

TEST(camera_helper, get_viewport_for_pixel_half_size_outside) {
	kengine::render::viewport viewport;
	viewport.bounding_box.size = { .5f, .5f };
	const auto result = get_viewport_for_pixel({
		.viewport_component = viewport,
		.pixel = { 640, 360 },
	});
	EXPECT_EQ(result.info.camera, entt::entity(entt::null));
	EXPECT_EQ(result.info.pixel.x, -1.f);
	EXPECT_EQ(result.info.pixel.y, -1.f);
	EXPECT_EQ(result.info.viewport_percent.x, -1.f);
	EXPECT_EQ(result.info.viewport_percent.y, -1.f);
}

TEST(camera_helper, get_viewport_for_pixel_half_size_inside) {
	kengine::render::viewport viewport;
	viewport.bounding_box.size = { .5f, .5f };
	const auto result = get_viewport_for_pixel({
		.viewport_component = viewport,
		.pixel = { 320, 180 },
	});
	EXPECT_EQ(result.info.camera, result.window);
	EXPECT_EQ(result.info.pixel.x, .25f);
	EXPECT_EQ(result.info.pixel.y, .25f);
	EXPECT_EQ(result.info.viewport_percent.x, .5f);
	EXPECT_EQ(result.info.viewport_percent.y, .5f);
}

TEST(camera_helper, get_viewport_for_pixel_offset_zero) {
	kengine::render::viewport viewport;
	viewport.bounding_box.position = { .5f, .5f };
	const auto result = get_viewport_for_pixel({
		.viewport_component = viewport,
		.pixel = { 640, 360 },
	});
	EXPECT_EQ(result.info.camera, result.window);
	EXPECT_EQ(result.info.pixel.x, .5f);
	EXPECT_EQ(result.info.pixel.y, .5f);
	EXPECT_EQ(result.info.viewport_percent.x, 0.f);
	EXPECT_EQ(result.info.viewport_percent.y, 0.f);
}

TEST(camera_helper, get_viewport_for_pixel_offset_outside) {
	kengine::render::viewport viewport;
	viewport.bounding_box.position = { .5f, .5f };
	const auto result = get_viewport_for_pixel({
		.viewport_component = viewport,
		.pixel = { 0, 0 },
	});
	EXPECT_EQ(result.info.camera, entt::entity(entt::null));
	EXPECT_EQ(result.info.pixel.x, -1.f);
	EXPECT_EQ(result.info.pixel.y, -1.f);
	EXPECT_EQ(result.info.viewport_percent.x, -1.f);
	EXPECT_EQ(result.info.viewport_percent.y, -1.f);
}

TEST(camera_helper, get_viewport_for_pixel_offset_inside) {
	kengine::render::viewport viewport;
	viewport.bounding_box.position = { .5f, .5f };
	const auto result = get_viewport_for_pixel({
		.viewport_component = viewport,
		.pixel = { 960, 540 },
	});
	EXPECT_EQ(result.info.camera, result.window);
	EXPECT_EQ(result.info.pixel.x, .75f);
	EXPECT_EQ(result.info.pixel.y, .75f);
	EXPECT_EQ(result.info.viewport_percent.x, .25f);
	EXPECT_EQ(result.info.viewport_percent.y, .25f);
}