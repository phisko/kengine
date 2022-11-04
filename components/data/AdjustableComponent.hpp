#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
# define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

// stl
#include <variant>
#include <vector>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "string.hpp"
#include "Color.hpp"
#include "lengthof.hpp"

namespace kengine {
	struct AdjustableComponent {
		static constexpr char stringName[] = "AdjustableComponentString";
		using string = putils::string<KENGINE_ADJUSTABLE_NAME_MAX_LENGTH, stringName>;
		struct Value;

		string section;
		std::vector<Value> values;

		struct Value {
			template<typename T, const char * Name, typename = std::enable_if_t < !std::is_pointer<T>{} >>
				struct Storage {
				Storage() noexcept = default;
				explicit Storage(T * ptr) noexcept : ptr(ptr), value(*ptr) {}
				explicit Storage(T value) noexcept : value(value) {}
				T * ptr = nullptr;
				T value;
			};

			static constexpr char _boolStorageName[] = "AdjustableComponentStorageBool";
			using BoolStorage = Storage<bool, _boolStorageName>;

			static constexpr char _floatStorageName[] = "AdjustableComponentStorageFloat";
			using FloatStorage = Storage<float, _floatStorageName>;

			static constexpr char _intStorageName[] = "AdjustableComponentStorageInt";
			using IntStorage = Storage<int, _intStorageName>;

			static constexpr char _colorStorageName[] = "AdjustableComponentStorageColor";
			using ColorStorage = Storage<putils::NormalizedColor, _colorStorageName>;

			Value() noexcept {}
			Value(const Value &) noexcept = default;
			Value & operator=(const Value &) noexcept = default;
			Value(Value &&) noexcept = default;
			Value & operator=(Value &&) noexcept = default;

			Value(const char * name, bool * b) noexcept : name(name), boolStorage(b), type(Type::Bool) {}
			Value(const char * name, bool b) noexcept : name(name), boolStorage(b), type(Type::Bool) {}

			Value(const char * name, float * f) noexcept : name(name), floatStorage(f), type(Type::Float) {}
			Value(const char * name, float f) noexcept : name(name), floatStorage(f), type(Type::Float) {}

			Value(const char * name, int * i) noexcept : name(name), intStorage(i), type(Type::Int) {}
			Value(const char * name, int i) noexcept : name(name), intStorage(i), type(Type::Int) {}

			Value(const char * name, putils::NormalizedColor * color) noexcept : name(name), colorStorage(color), type(Type::Color) {}
			Value(const char * name, putils::NormalizedColor color) noexcept : name(name), colorStorage(color), type(Type::Color) {}
			Value(const char * name, putils::Color color) noexcept : name(name), colorStorage(putils::toNormalizedColor(color)), type(Type::Color) {}

			template<typename E>
			static const char ** getEnumNamesImpl() noexcept {
				static putils::string<64> names[magic_enum::enum_count<E>()];
				static const char * ret[putils::lengthof(names)];
				static bool first = true;
				if (first) {
					for (size_t i = 0; i < putils::lengthof(names); ++i) {
						names[i] = magic_enum::enum_names<E>()[i];
						ret[i] = names[i].c_str();
					}
					first = false;
				}
				return ret;
			}

			template<typename E>
			Value(const char * name, E * enumType) noexcept : name(name), intStorage((int *)enumType), type(Type::Int) {
				static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
				getEnumNames = getEnumNamesImpl<E>;
				enumCount = magic_enum::enum_count<E>();
			}

			template<typename E>
			Value(const char * name, E enumType) noexcept : name(name), intStorage((int)enumType), type(Type::Int) {
				static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
				getEnumNames = getEnumNamesImpl<E>();
				enumCount = magic_enum::enum_count<E>();
			}

			string name;

			union {
                BoolStorage boolStorage;
                FloatStorage floatStorage;
                IntStorage intStorage;
                ColorStorage colorStorage;
            };

            enum class Type {
                Bool,
                Float,
                Int,
                Color,
                Invalid
            };

            Type type = Type::Invalid;

			using EnumNameFunc = const char ** ();
			EnumNameFunc * getEnumNames = nullptr;
			size_t enumCount = 0;
		};
	};
}

#define refltype kengine::AdjustableComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(section),
		putils_reflection_attribute(values)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(refltype::Value)
	);
};
#undef refltype

#define refltype kengine::AdjustableComponent::Value
putils_reflection_info {
	putils_reflection_custom_class_name(AdjustableComponentValue);
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(boolStorage),
        putils_reflection_attribute(floatStorage),
        putils_reflection_attribute(intStorage),
        putils_reflection_attribute(colorStorage),
        putils_reflection_attribute(type)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::BoolStorage),
		putils_reflection_type(refltype::FloatStorage),
		putils_reflection_type(refltype::IntStorage),
		putils_reflection_type(refltype::ColorStorage)
	);
};
#undef refltype

template<typename T, const char * Name, typename E>
#define refltype kengine::AdjustableComponent::Value::Storage<T, Name, E>
putils_reflection_info_template {
	static constexpr auto class_name = Name;
	putils_reflection_attributes(
		putils_reflection_attribute(value)
	);
};
#undef refltype

