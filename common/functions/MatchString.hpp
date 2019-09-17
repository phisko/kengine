#pragma once

namespace kengine {
	class Entity;

	namespace functions {
		struct MatchString {
			using Signature = bool(*)(const Entity &, const char *);
			Signature funcPtr;
		};
	}
}