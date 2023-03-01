// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/render/functions/appears_in_viewport.hpp"
#include "kengine/render/helpers/entity_appears_in_viewport.hpp"

TEST(render, entity_appears_in_viewport_true) {
	entt::registry r;
	const auto e = r.create();
	const auto viewport = r.create();
	EXPECT_TRUE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_entity_refuses) {
	entt::registry r;
	const auto e = r.create();
	const auto viewport = r.create();

	r.emplace<kengine::render::appears_in_viewport>(e, [](entt::entity e) {
		return false;
	});
	EXPECT_FALSE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_viewport_refuses) {
	entt::registry r;
	const auto e = r.create();

	const auto viewport = r.create();
	r.emplace<kengine::render::appears_in_viewport>(viewport, [](entt::entity e) {
		return false;
	});

	EXPECT_FALSE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_both_refuse) {
	entt::registry r;
	const auto e = r.create();
	r.emplace<kengine::render::appears_in_viewport>(e, [](entt::entity e) {
		return false;
	});

	const auto viewport = r.create();
	r.emplace<kengine::render::appears_in_viewport>(viewport, [](entt::entity e) {
		return false;
	});

	EXPECT_FALSE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_entity_accepts) {
	entt::registry r;
	const auto e = r.create();
	r.emplace<kengine::render::appears_in_viewport>(e, [](entt::entity e) {
		return true;
	});

	const auto viewport = r.create();

	EXPECT_TRUE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_viewport_accepts) {
	entt::registry r;
	const auto e = r.create();

	const auto viewport = r.create();
	r.emplace<kengine::render::appears_in_viewport>(viewport, [](entt::entity e) {
		return true;
	});

	EXPECT_TRUE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_both_accept) {
	entt::registry r;
	const auto e = r.create();
	r.emplace<kengine::render::appears_in_viewport>(e, [](entt::entity e) {
		return true;
	});

	const auto viewport = r.create();
	r.emplace<kengine::render::appears_in_viewport>(viewport, [](entt::entity e) {
		return true;
	});

	EXPECT_TRUE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_entity_accepts_viewport_refuses) {
	entt::registry r;
	const auto e = r.create();
	r.emplace<kengine::render::appears_in_viewport>(e, [](entt::entity e) {
		return true;
	});

	const auto viewport = r.create();
	r.emplace<kengine::render::appears_in_viewport>(viewport, [](entt::entity e) {
		return false;
	});

	EXPECT_FALSE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}

TEST(render, entity_appears_in_viewport_entity_refuses_viewport_accepts) {
	entt::registry r;
	const auto e = r.create();
	r.emplace<kengine::render::appears_in_viewport>(e, [](entt::entity e) {
		return false;
	});

	const auto viewport = r.create();
	r.emplace<kengine::render::appears_in_viewport>(viewport, [](entt::entity e) {
		return true;
	});

	EXPECT_FALSE(kengine::render::entity_appears_in_viewport(r, e, viewport));
}
