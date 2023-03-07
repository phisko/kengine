#pragma once

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/instance/data/instance.hpp"

struct instance : ::testing::Test {
	instance() noexcept {
		model = { r, r.create() };
		model.emplace<std::string>("hello");

		e = { r, r.create() };
		e.emplace<kengine::instance::instance>(model);
	}

	entt::registry r;
	entt::handle model;
	entt::handle e;
};
