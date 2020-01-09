#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
# define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

#include <vector>
#include "string.hpp"
#include "Color.hpp"
#include "magic_enum.hpp"
#include "lengthof.hpp"

namespace kengine {
	struct AdjustableComponent {
		static constexpr char stringName[] = "AdjustableComponentString";
		using string = putils::string<KENGINE_ADJUSTABLE_NAME_MAX_LENGTH, stringName>;

		struct Value {
			Value() = default;

			Value(const char * name, bool * b) : name(name), b(b), adjustableType(Bool) {}
			Value(const char * name, bool b) : name(name), b(b), adjustableType(Bool) {}

			Value(const char * name, float * f) : name(name), f(f), adjustableType(Double) {}
			Value(const char * name, float f) : name(name), f(f), adjustableType(Double) {}

			Value(const char * name, int * i) : name(name), i(i), adjustableType(Int) {}
			Value(const char * name, int i) : name(name), i(i), adjustableType(Int) {}

			Value(const char * name, putils::NormalizedColor * color) : name(name), color(color), adjustableType(Color) {}
			Value(const char * name, putils::NormalizedColor color) : name(name), color(color), adjustableType(Color) {}
			Value(const char * name, putils::Color color) : name(name), color(putils::toNormalizedColor(color)), adjustableType(Color) {}

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

			template<typename E>
			Value(const char * name, E * enumType) : name(name), i((int *)enumType), adjustableType(Enum) {
				static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
				getEnumNames = getEnumNamesImpl<E>;
				enumCount = putils::magic_enum::enum_count<E>();
			}

			template<typename E>
			Value(const char * name, E enumType) : name(name), i(enumType), adjustableType(Enum) {
				static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
				getEnumNames = getEnumNamesImpl<E>();
				enumCount = putils::magic_enum::enum_count<E>();
			}

			string name;

			template<typename T, const char * Name, typename = std::enable_if_t<!std::is_pointer<T>{}>>
			struct Storage {
				Storage() = default;
				explicit Storage(T * ptr) : ptr(ptr), value(*ptr) {}
				explicit Storage(T value) : value(value) {}
				T * ptr = nullptr;
				T value;

				static const auto reflection_get_class_name() { return Name; }
				putils_reflection_attributes(
					putils_reflection_attribute(&Storage::value)
				);
			};

			static constexpr char _boolStorageName[] = "AdjustableComponentStorageBool";
			using BoolStorage = Storage<bool, _boolStorageName>;
			BoolStorage b;

			static constexpr char _floatStorageName[] = "AdjustableComponentStorageFloat";
			using FloatStorage = Storage<float, _floatStorageName>;
			FloatStorage f;

			static constexpr char _intStorageName[] = "AdjustableComponentStorageInt";
			using IntStorage = Storage<int, _intStorageName>;
			IntStorage i;

			static constexpr char _colorStorageName[] = "AdjustableComponentStorageColor";
			using ColorStorage = Storage<putils::NormalizedColor, _colorStorageName>;
			ColorStorage color;

			using EnumNameFunc = const char ** ();
			EnumNameFunc * getEnumNames = nullptr;
			size_t enumCount = 0;

			enum EType {
				Bool,
				Double,
				Int,
				Color,
				Enum
			};

			EType adjustableType = Bool;

			putils_reflection_class_name(AdjustableComponentValue);
			putils_reflection_attributes(
				putils_reflection_attribute(&Value::name),
				putils_reflection_attribute(&Value::b),
				putils_reflection_attribute(&Value::f),
				putils_reflection_attribute(&Value::i),
				putils_reflection_attribute(&Value::color),
				putils_reflection_attribute(&Value::adjustableType)
			);
		};

		string section;
		std::vector<Value> values;

		putils_reflection_class_name(AdjustableComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AdjustableComponent::values)
		);
	};
}