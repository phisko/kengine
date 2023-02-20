// gtest
#include <gtest/gtest.h>

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/helpers/new_entity_processor.hpp"

struct core : ::testing::Test {
	entt::registry r;

	struct processed_entity {
		entt::entity e = entt::null;
		int i = 0;
		double d = 0;
	};
	std::vector<processed_entity> processed_entities;

	const std::vector<processed_entity> created_entities {
		{ r.create(), 42, 84 },
		{ r.create(), -42, -84 }
	};

	void create_entities() noexcept {
		for (const auto & e : created_entities) {
			r.emplace<int>(e.e, e.i);
			r.emplace<double>(e.e, e.d);
		}
	}

	void check_equality() noexcept {
		EXPECT_EQ(processed_entities.size(), created_entities.size());
		for (size_t i = 0; i < created_entities.size(); ++i) {
			const auto & processed = processed_entities[i];
			const auto created = std::ranges::find_if(created_entities, [&](const auto & e) { return e.e == processed.e; });
			EXPECT_NE(created, created_entities.end());
			EXPECT_EQ(processed.e, created->e);
			EXPECT_EQ(processed.i, created->i);
			EXPECT_EQ(processed.d, created->d);
		}
	}
};

TEST_F(core, new_entity_processor_created_before) {
	struct processed {};
	kengine::new_entity_processor<processed, int, double> processor{ r, [&](entt::entity e, int i, double d) {
		processed_entities.push_back({ e, i, d });
	}};

	create_entities();
	EXPECT_EQ(processed_entities.size(), 0);
	processor.process();
	check_equality();
}

TEST_F(core, new_entity_processor_created_after) {
	create_entities();

	struct processed {};
	kengine::new_entity_processor<processed, int, double> processor{ r, [&](entt::entity e, int i, double d) {
		processed_entities.push_back({ e, i, d });
	}};
	EXPECT_EQ(processed_entities.size(), 0);
	processor.process();
	check_equality();
}