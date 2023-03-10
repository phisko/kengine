#include "system.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/scn/scn.hpp"
#include "putils/split.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/command_line/data/arguments.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/functions/has_metadata.hpp"
#include "kengine/meta/json/functions/load.hpp"
#include "kengine/meta/json/functions/save.hpp"

#ifndef KENGINE_CONFIG_SAVE_FILE
#define KENGINE_CONFIG_SAVE_FILE "config.json"
#endif

namespace kengine::config::json {
	static constexpr auto log_category = "config_json";
	static constexpr auto config_metadata = "config";

	struct system {
		entt::registry & r;

		// Loaded from and saved to KENGINE_CONFIG_SAVE_FILE
		nlohmann::json file_config = load_json_file();

		// With temporary modifications (such as command-line overrides)
		nlohmann::json transient_config = process_command_line();

		// Initialize new config values with the JSON contents
		struct processed {};
		kengine::new_entity_processor<processed, core::name, configurable> processor{ r, putils_forward_to_this(on_construct_config) };

		// Save to the JSON file when config values change
		const entt::scoped_connection connection = r.on_update<configurable>().connect<&system::save>(this);

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

		void on_construct_config(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & name = r.get<core::name>(e);
			kengine_logf(r, verbose, log_category, "Initializing {} ({})", e, name.name);

			auto * current_json_section = &transient_config;
			const auto sections = putils::split(name.name, '/');
			for (const auto section : sections) {
				const auto it = current_json_section->find(std::string(section));
				if (it == current_json_section->end()) {
					kengine_logf(r, warning, log_category, "Section '{}' not found in JSON file", name.name);
					return;
				}
				current_json_section = &*it;
			}

			for (const auto & [type_entity, load_json, has_metadata] : r.view<meta::json::load, meta::has_metadata>().each())
				if (has_metadata(config_metadata))
					load_json(*current_json_section, { r, e });
		}

		nlohmann::json load_json_file() const noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Loading from " KENGINE_CONFIG_SAVE_FILE);

			std::ifstream f(KENGINE_CONFIG_SAVE_FILE);
			if (!f) {
				kengine_log(r, verbose, log_category, "Failed to open " KENGINE_CONFIG_SAVE_FILE);
				return {};
			}

			return nlohmann::json::parse(f);
		}

		nlohmann::json process_command_line() const noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Overriding through command-line");

			nlohmann::json ret = file_config;

			bool dump_config = false;
			for (const auto & [e, command_line] : r.view<command_line::arguments>().each())
				for (const auto arg : command_line.args) {
					constexpr const auto dump_flag = "--dump-config";
					if (arg.starts_with(dump_flag))
						dump_config = true;

					constexpr const std::string_view flag = "--config:";
					if (!arg.starts_with(flag))
						continue;

					kengine_logf(r, verbose, log_category, "Found override: '{}'", arg);
					const auto key_value = arg.substr(flag.size());
					const auto equal_pos = key_value.find('=');
					if (equal_pos == std::string_view::npos) {
						kengine_assert_failed(r, "Bad format for command-line option '{}'", arg);
						continue;
					}

					const auto key = key_value.substr(0, equal_pos);
					const auto value = key_value.substr(equal_pos + 1);
					kengine_logf(r, verbose, log_category, "Overriding '{}' with '{}'", key, value);
					try {
						const nlohmann::json::json_pointer ptr{ std::string(key) };
						ret[ptr] = nlohmann::json::parse(value);
					}
					catch (const nlohmann::json::parse_error & e) {
						kengine_assert_failed(r, "Error in command-line option '{}': {}", arg, e.what());
					}
				}

			if (dump_config)
				kengine_log(r, log, log_category, fmt::format("Config: {}", ret.dump(4)).c_str());

			return ret;
		}

		void save(entt::registry &, entt::entity changed_entity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Saving to " KENGINE_CONFIG_SAVE_FILE " because {} changed", changed_entity);

			std::ofstream f(KENGINE_CONFIG_SAVE_FILE, std::ofstream::trunc);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '" KENGINE_CONFIG_SAVE_FILE "' with write permissions");
				return;
			}

			const auto & section_name = r.get<core::name>(changed_entity);

			auto * current_json_section = &file_config;
			const auto sections = putils::split(section_name.name, '/');
			for (const auto section : sections)
				current_json_section = &(*current_json_section)[section];

			for (const auto & [type_entity, name, has, save_json, has_metadata] : r.view<core::name, meta::has, meta::json::save, meta::has_metadata>().each())
				if (has({ r, changed_entity }) && has_metadata(config_metadata))
					current_json_section->merge_patch({
						{ name.name, save_json({ r, changed_entity }) }
					});

			f << file_config.dump(4);
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}
