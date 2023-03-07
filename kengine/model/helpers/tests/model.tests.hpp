#pragma once

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gtest
#include <gtest/gtest.h>

// kengine
#include "kengine/model/data/instance.hpp"

struct model : ::testing::Test {
	model() noexcept {
		m = { r, r.create() };
		m.emplace<std::string>("hello");

		e = { r, r.create() };
		e.emplace<kengine::model::instance>(m);
	}

	entt::registry r;
	entt::handle m;
	entt::handle e;
};
