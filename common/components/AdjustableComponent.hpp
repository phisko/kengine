#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
# define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

#include "string.hpp"
#include "reflection.hpp"
#include "Color.hpp"
#include "magic_enum.hpp"
#include "lengthof.hpp"

namespace kengine {
	class AdjustableComponent : kengine::not_serializable {
	public:
		static constexpr char stringName[] = "AdjustableComponentString";
		using string = putils::string<KENGINE_ADJUSTABLE_NAME_MAX_LENGTH, stringName>;

	private:
		template<typename E>
		static const char ** getEnumNamesImpl() {
			static putils::string<64> names[putils::magic_enum::enum_count<E>()];
			static const char * ret[lengthof(names)];
			static bool first = true;
			if (first) {
				for (size_t i = 0; i < lengthof(names); ++i) {
					names[i] = putils::magic_enum::enum_names<E>()[i];
					ret[i] = names[i];
				}
				first = false;
			}
			return ret;
		}

	public:
		AdjustableComponent() = default;

		AdjustableComponent(const char * name, bool * b) : name(name), bPtr(b), b(*b), adjustableType(Bool) {}
		AdjustableComponent(const char * name, bool b) : name(name), b(b), adjustableType(Bool) {}

		AdjustableComponent(const char * name, float * d) : name(name), dPtr(d), d(*d), adjustableType(Double) {}
		AdjustableComponent(const char * name, float d) : name(name), d(d), adjustableType(Double) {}

		AdjustableComponent(const char * name, int * i) : name(name), iPtr(i), i(*i), adjustableType(Int) {}
		AdjustableComponent(const char * name, int i) : name(name), i(i), adjustableType(Int) {}

		AdjustableComponent(const char * name, putils::NormalizedColor * color) : name(name), colorPtr(color), color(*color), adjustableType(Color) {}
		AdjustableComponent(const char * name, putils::NormalizedColor color) : name(name), color(color), adjustableType(Color) {}
		AdjustableComponent(const char * name, putils::Color color) : name(name), color(putils::toNormalizedColor(color)), adjustableType(Color) {}

		template<typename E>
		AdjustableComponent(const char * name, E * enumType) : name(name), iPtr((int*)enumType), i(*(int *)enumType), adjustableType(Enum) {
			static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
			getEnumNames = getEnumNamesImpl<E>;
			enumCount = putils::magic_enum::enum_count<E>();
		}

		template<typename E>
		AdjustableComponent(const char * name, E enumType) : name(name), i(enumType), adjustableType(Enum) {
			static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
			getEnumNames = getEnumNamesImpl<E>();
			enumCount = putils::magic_enum::enum_count<E>();
		}

		string name;

		bool * bPtr = nullptr;
		float * dPtr = nullptr;
		int * iPtr = nullptr;
		putils::NormalizedColor * colorPtr = nullptr;

		bool b = false;
		float d = 0.f;
		int i = 0;
		putils::NormalizedColor color;

		using EnumNameFunc = const char ** ();
		EnumNameFunc * getEnumNames = nullptr;
		size_t enumCount;

		enum EType {
			Bool,
			Double,
			Int,
			Color,
			Enum
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