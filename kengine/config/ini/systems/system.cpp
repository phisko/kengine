#include "system.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/ini_file.hpp"
#include "putils/scn/scn.hpp"
#include "putils/split.hpp"

// kengine
#include "kengine/config/data/values.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"

#ifndef KENGINE_CONFIG_SAVE_FILE
#define KENGINE_CONFIG_SAVE_FILE "config.ini"
#endif

namespace kengine::config::ini {
	static constexpr auto log_category = "config_ini";

	struct system {
		entt::registry & r;
		putils::ini_file loaded_file = load_ini_file();

		// Initialize new config values with the INI contents
		struct processed {};
		kengine::new_entity_processor<processed, values> processor{ r, putils_forward_to_this(on_construct_config) };

		// Save to the INI file when config values change
		const entt::scoped_connection connection = r.on_update<values>().connect<&system::save>(this);

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			processor.process();
		}

		void on_construct_config(entt::entity e, values & comp) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Initializing section {}", comp.section);

			auto it = loaded_file.sections.cend();
			const auto sections = putils::split(comp.section, '/');
			for (const auto section : sections) {
				const auto & current_sections = it == loaded_file.sections.end()
					? loaded_file.sections
					: it->second.sections;

				it = current_sections.find(std::string(section));
				if (it == current_sections.end()) {
					kengine_logf(r, warning, log_category, "Section '{}' not found in INI file", comp.section);
					return;
				}
			}

			const auto & section = it->second;
			for (auto & value : comp.entries) {
				const auto it = section.values.find(value.name.c_str());
				if (it == section.values.end()) {
					kengine_logf(r, warning, log_category, "Value for '{}' not found in INI file", value.name);
					continue;
				}

				kengine_logf(r, verbose, log_category, "Initializing {} to {}", value.name, it->second);
				set_value(value, it->second.c_str());
			}
		}

		void set_value(values::value & value, const char * s) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto assign_ptr = [](auto & storage) {
				if (storage.ptr != nullptr)
					*storage.ptr = storage.value;
			};

			const std::string_view view(s);
			switch (value.type) {
				case values::value_type::Int: {
					const auto result = scn::scan_default(view, value.int_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.int_storage);
					break;
				}
				case values::value_type::Float: {
					const auto result = scn::scan_default(view, value.float_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.float_storage);
					break;
				}
				case values::value_type::Bool: {
					const auto result = scn::scan_default(view, value.bool_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.bool_storage);
					break;
				}
				case values::value_type::Color: {
					const auto result = scn::scan_default(view, value.color_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.color_storage);
					break;
				}
				default: {
					static_assert(magic_enum::enum_count<values::value_type>() == 5); // + 1 for Invalid
					kengine_assert_failed(r, "Unknown values::value type");
					break;
				}
			}
		}

		putils::ini_file load_ini_file() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Loading from " KENGINE_CONFIG_SAVE_FILE);

			std::ifstream f(KENGINE_CONFIG_SAVE_FILE);

			putils::ini_file ret;
			f >> ret;
			return ret;
		}

		void save(entt::registry &, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Saving to " KENGINE_CONFIG_SAVE_FILE " because {} changed", e);

			std::ofstream f(KENGINE_CONFIG_SAVE_FILE, std::ofstream::trunc);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '" KENGINE_CONFIG_SAVE_FILE "' with write permissions");
				return;
			}

			putils::ini_file ini;

			for (const auto & [e, comp] : r.view<values>().each()) {
				kengine_logf(r, verbose, log_category, "Adding section {} to INI file", e, comp.section);
				auto & section = ini.sections[comp.section.c_str()];

				for (const auto & value : comp.entries) {
					kengine_logf(r, verbose, log_category, "Adding value {} to section {} of INI file", e, value.name);
					auto & ini_value = section.values[value.name.c_str()];

					switch (value.type) {
						case values::value_type::Int: {
							ini_value = fmt::format("{}", value.int_storage.value);
							kengine_logf(r, verbose, log_category, "Adding int value {}", e, ini_value);
							break;
						}
						case values::value_type::Float: {
							ini_value = fmt::format("{}", value.float_storage.value);
							kengine_logf(r, verbose, log_category, "Adding float value {}", e, ini_value);
							break;
						}
						case values::value_type::Bool: {
							ini_value = fmt::format("{}", value.bool_storage.value);
							kengine_logf(r, verbose, log_category, "Adding bool value {}", e, ini_value);
							break;
						}
						case values::value_type::Color: {
							ini_value = fmt::format("{}", value.color_storage.value);
							kengine_logf(r, verbose, log_category, "Adding color value {}", e, ini_value);
							break;
						}
						default: {
							static_assert(magic_enum::enum_count<values::value_type>() == 5); // + 1 for Invalid
							kengine_assert_failed(r, "Unknown values::value type");
							break;
						}
					}
				}
			}

			f << ini;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}
