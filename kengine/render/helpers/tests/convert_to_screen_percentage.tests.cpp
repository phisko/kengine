// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/render/helpers/convert_to_screen_percentage.hpp"

TEST(render, convert_to_screen_percentage_percentage) {
	kengine::render::on_screen comp;
	comp.coordinates = kengine::render::on_screen::coordinate_type::screen_percentage;
	const putils::rect2f rect{ { .25f, .25f }, { .5f, .5f } };
	EXPECT_EQ(kengine::render::convert_to_screen_percentage(rect, { 1280, 720 }, comp), rect);
}

TEST(camera_helper, convert_to_screen_percentage_pixels) {
	kengine::render::on_screen comp;
	comp.coordinates = kengine::render::on_screen::coordinate_type::pixels;

	const putils::rect2f rect{ { 0.f, 0.f }, { 640.f, 360.f } };
	const auto screen_percentage = kengine::render::convert_to_screen_percentage(rect, { 1280, 720 }, comp);

	const auto expected_pos = putils::point2f{ 0.f, 0.f };
	EXPECT_EQ(screen_percentage.position, expected_pos);

	const auto expected_size = putils::point2f{ .5f, .5f };
	EXPECT_EQ(screen_percentage.size, expected_size);
}
