#pragma once

#ifndef KENGINE_ADJUSTABLE_NAME_MAX_LENGTH
#define KENGINE_ADJUSTABLE_NAME_MAX_LENGTH 64
#endif

// stl
#include <variant>
#include <vector>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"
#include "putils/lengthof.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::string, refltype::value]
	struct adjustable {
		static constexpr char string_name[] = "adjustable_string";
		using string = putils::string<KENGINE_ADJUSTABLE_NAME_MAX_LENGTH, string_name>;
		struct value;

		string section;
		std::vector<value> values;

		enum class value_type {
			Bool,
			Float,
			Int,
			Color,
			Invalid
		};

		/*!
		 * putils reflect all
		 * class_name: adjustable_value
		 * used_types: [
		 * 		refltype::storage_for_bool,
		 * 		refltype::storage_for_float,
		 * 		refltype::storage_for_int,
		 * 		refltype::storage_for_color,
		 * 		putils::normalized_color
		 * ]
		 */
		struct value {
			template<typename T, const char * Name>
			struct storage {
				storage() noexcept = default;
				explicit storage(T * ptr) noexcept : ptr(ptr), value(*ptr) {}
				explicit storage(T value) noexcept : value(value) {}
				T * ptr = nullptr;
				T value;
			};

			static constexpr char _bool_storage_name[] = "adjustable_storage_bool";
			using storage_for_bool = storage<bool, _bool_storage_name>;

			static constexpr char _float_storage_name[] = "adjustable_storage_float";
			using storage_for_float = storage<float, _float_storage_name>;

			static constexpr char _int_storage_name[] = "adjustable_storage_int";
			using storage_for_int = storage<int, _int_storage_name>;

			static constexpr char _color_storage_name[] = "adjustable_storage_color";
			using storage_for_color = storage<putils::normalized_color, _color_storage_name>;

			value() noexcept {}
			value(const value &) noexcept = default;
			value & operator=(const value &) noexcept = default;
			value(value &&) noexcept = default;
			value & operator=(value &&) noexcept = default;

			value(const char * name, bool * b) noexcept : name(name), bool_storage(b), type(value_type::Bool) {}
			value(const char * name, bool b) noexcept : name(name), bool_storage(b), type(value_type::Bool) {}

			value(const char * name, float * f) noexcept : name(name), float_storage(f), type(value_type::Float) {}
			value(const char * name, float f) noexcept : name(name), float_storage(f), type(value_type::Float) {}

			value(const char * name, int * i) noexcept : name(name), int_storage(i), type(value_type::Int) {}
			value(const char * name, int i) noexcept : name(name), int_storage(i), type(value_type::Int) {}

			value(const char * name, putils::normalized_color * color) noexcept : name(name), color_storage(color), type(value_type::Color) {}
			value(const char * name, putils::normalized_color color) noexcept : name(name), color_storage(color), type(value_type::Color) {}
			value(const char * name, putils::color color) noexcept : name(name), color_storage(putils::to_normalized_color(color)), type(value_type::Color) {}

			template<typename E>
			static const char ** get_enum_names_impl() noexcept {
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
			value(const char * name, E * enum_type) noexcept : name(name), int_storage((int *)enum_type), type(value_type::Int) {
				static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
				get_enum_names = get_enum_names_impl<E>;
				enum_count = magic_enum::enum_count<E>();
			}

			template<typename E>
			value(const char * name, E enum_type) noexcept : name(name), int_storage((int)enum_type), type(value_type::Int) {
				static_assert(std::is_enum_v<E> && std::is_same_v<std::underlying_type_t<E>, int>);
				get_enum_names = get_enum_names_impl<E>();
				enum_count = magic_enum::enum_count<E>();
			}

			string name;

			union {
				storage_for_bool bool_storage;
				storage_for_float float_storage;
				storage_for_int int_storage;
				storage_for_color color_storage;
			};

			value_type type = value_type::Invalid;

			using enum_name_func = const char **();
			//! putils reflect off
			enum_name_func * get_enum_names = nullptr;
			size_t enum_count = 0;
		};
	};
}

#include "adjustable.reflection.hpp"

template<typename T, const char * Name>
#define refltype kengine::data::adjustable::value::storage<T, Name>
putils_reflection_info_template {
	static constexpr auto class_name = Name;
	putils_reflection_attributes(
		putils_reflection_attribute(value)
	);
};
#undef refltype
