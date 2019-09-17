#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
# define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

#include "string.hpp"
#include "reflection/Reflectible.hpp"
#include "Color.hpp"

namespace kengine {
	class AdjustableComponent : kengine::not_serializable {
	public:
		static constexpr char stringName[] = "AdjustableComponentString";
		using string = putils::string<KENGINE_ADJUSTABLE_NAME_MAX_LENGTH, stringName>;

	public:
		AdjustableComponent() = default;

		AdjustableComponent(const char * name, bool * b) : name(name), bPtr(b), b(*b), adjustableType(Bool) {}
		AdjustableComponent(const char * name, float * d) : name(name), dPtr(d), d(*d), adjustableType(Double) {}
		AdjustableComponent(const char * name, int * i) : name(name), iPtr(i), i(*i), adjustableType(Int) {}
		AdjustableComponent(const char * name, putils::NormalizedColor * color) : name(name), colorPtr(color), color(*color), adjustableType(Color) {}

		AdjustableComponent(const char * name, bool b) : name(name), b(b), adjustableType(Bool) {}
		AdjustableComponent(const char * name, float d) : name(name), d(d), adjustableType(Double) {}
		AdjustableComponent(const char * name, int i) : name(name), i(i), adjustableType(Int) {}
		AdjustableComponent(const char * name, putils::NormalizedColor color) : name(name), color(color), adjustableType(Color) {}
		AdjustableComponent(const char * name, putils::Color color) : name(name), color(putils::toNormalizedColor(color)), adjustableType(Color) {}

		string name;

		bool * bPtr = nullptr;
		float * dPtr = nullptr;
		int * iPtr = nullptr;
		putils::NormalizedColor * colorPtr = nullptr;

		bool b = false;
		float d = 0.f;
		int i = 0;
		putils::NormalizedColor color;

		enum EType {
			Bool,
			Double,
			Int,
			Color
		};

		EType adjustableType = Bool;

		pmeta_get_class_name(AdjustableComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AdjustableComponent::name),
			pmeta_reflectible_attribute(&AdjustableComponent::b),
			pmeta_reflectible_attribute(&AdjustableComponent::d),
			pmeta_reflectible_attribute(&AdjustableComponent::i),
			pmeta_reflectible_attribute(&AdjustableComponent::color),
			pmeta_reflectible_attribute(&AdjustableComponent::adjustableType)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}