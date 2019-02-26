#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
# define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

#define PUTILS_STRING_LENGTH KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
#include "string.hpp"
#undef PUTILS_STRING_LENGTH

#include "reflection/Reflectible.hpp"

namespace kengine {
	class AdjustableComponent {
	public:
		using string = putils::string<KENGINE_ADJUSTABLE_NAME_MAX_LENGTH>;

	public:
		AdjustableComponent() = default;

		AdjustableComponent(const char * name, bool * b) : name(name), bPtr(b), b(*b), adjustableType(Bool) {}
		AdjustableComponent(const char * name, float * d) : name(name), dPtr(d), d(*d), adjustableType(Double) {}
		AdjustableComponent(const char * name, int * i) : name(name), iPtr(i), i(*i), adjustableType(Int) {}

		AdjustableComponent(const char * name, bool b) : name(name), b(b), adjustableType(Bool) {}
		AdjustableComponent(const char * name, float d) : name(name), d(d), adjustableType(Double) {}
		AdjustableComponent(const char * name, int i) : name(name), i(i), adjustableType(Int) {}

		string name;

		bool * bPtr = nullptr;
		float * dPtr = nullptr;
		int * iPtr = nullptr;

		bool b;
		float d;
		int i;

		enum EType {
			Bool,
			Double,
			Int
		};

		int adjustableType;

		pmeta_get_class_name(AdjustableComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AdjustableComponent::name),
			pmeta_reflectible_attribute(&AdjustableComponent::b),
			pmeta_reflectible_attribute(&AdjustableComponent::d),
			pmeta_reflectible_attribute(&AdjustableComponent::i),
			pmeta_reflectible_attribute(&AdjustableComponent::adjustableType)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}