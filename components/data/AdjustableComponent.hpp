#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
# define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

#include <variant>
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
			Value(const Value &) = default;
			Value & operator=(const Value &) = default;
			Value(Value &&) = default;
			Value & operator=(Value &&) = default;

			Value(const char * name, bool * b) : name(name), storage(BoolStorage{ b }) {}
			Value(const char * name, bool b) : name(name), storage(BoolStorage{ b }) {}

			Value(const char * name, float * f) : name(name), storage(FloatStorage{ f }) {}
			Value(const char * name, float f) : name(name), storage(FloatStorage{ f }) {}

			Value(const char * name, int * i) : name(name), storage(IntStorage{ i }) {}
			Value(const char * name, int i) : name(name), storage(IntStorage{ i }) {}

			Value(const char * name, putils::NormalizedColor * color) : name(name), storage(ColorStorage{ color }) {}
			Value(const char * name, putils::NormalizedColor color) : name(name), storage(ColorStorage{ color }) {}
			Value(const char * name, putils::Color color) : name(name), storage(ColorStorage{ putils::toNormalizedColor(color) }) {}

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

			static constexpr char _floatStorageName[] = "AdjustableComponentStorageFloat";
			using FloatStorage = Storage<float, _floatStorageName>;

			static constexpr char _intStorageName[] = "AdjustableComponentStorageInt";
			using IntStorage = Storage<int, _intStorageName>;

			static constexpr char _colorStorageName[] = "AdjustableComponentStorageColor";
			using ColorStorage = Storage<putils::NormalizedColor, _colorStorageName>;

			std::variant<BoolStorage, FloatStorage, IntStorage, ColorStorage> storage;

			using EnumNameFunc = const char ** ();
			EnumNameFunc * getEnumNames = nullptr;
			size_t enumCount = 0;

			putils_reflection_class_name(AdjustableComponentValue);
			putils_reflection_attributes(
				putils_reflection_attribute(&Value::name),
				putils_reflection_attribute(&Value::storage)
			);
			putils_reflection_used_types(
				putils_reflection_type(BoolStorage),
				putils_reflection_type(FloatStorage),
				putils_reflection_type(IntStorage),
				putils_reflection_type(ColorStorage)
			);
		};

		string section;
		std::vector<Value> values;

		putils_reflection_class_name(AdjustableComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AdjustableComponent::section),
			putils_reflection_attribute(&AdjustableComponent::values)
		);
		putils_reflection_used_types(
			putils_reflection_type(string),
			putils_reflection_type(Value)
		);
	};
}