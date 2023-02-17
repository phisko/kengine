#include "log_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/command_line_arguments.hpp"
#include "putils/split.hpp"
#include "putils/reflection_helpers/json_helper.hpp"

// kengine core
#include "kengine/core/helpers/assert_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine command_line
#include "kengine/command_line/data/arguments.hpp"

namespace {
	static kengine::log_severity max_log_severity = kengine::log_severity::none;

	struct options {
		kengine::log_severity log_level = kengine::log_severity::log;
		std::string log_category_levels;
	};
}

#define refltype options
putils_reflection_info {
	putils_reflection_custom_class_name(log);
	putils_reflection_attributes(
		putils_reflection_attribute(log_level),
		putils_reflection_attribute(log_category_levels)
	);
};
#undef refltype

namespace kengine::log_helper {
	static std::string severity_to_string(log_severity severity) noexcept {
		return std::string(magic_enum::enum_names<log_severity>()[int(severity)]);
	}

	void set_minimum_log_severity(const entt::registry & r, log_severity severity) noexcept {
		if (severity < max_log_severity) {
			max_log_severity = severity;
			kengine_assert(r, severity >= log_severity::KENGINE_LOG_MAX_SEVERITY);
		}
	}

	log_severity_control parse_command_line_severity(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "log_helper", "Parsing command-line severity");

		static std::optional<log_severity_control> command_line_severity;
		if (command_line_severity != std::nullopt) {
			kengine_logf(r, very_verbose, "log_helper", "Found pre-parsed %s", putils::reflection::to_json(*command_line_severity).dump(4).c_str());
			return *command_line_severity;
		}

		command_line_severity = log_severity_control{};

		for (const auto & [e, command_line] : r.view<command_line::arguments>().each()) {
			const auto opts = putils::parse_arguments<options>(command_line.args);
			kengine_logf(r, very_verbose, "log_helper", "Found %s in [%u]", putils::reflection::to_json(opts).dump(4).c_str(), e);

			command_line_severity->global_severity = opts.log_level;
			const auto categories = putils::split(opts.log_category_levels.c_str(), ',');
			for (const auto & category : categories) {
				const auto key_value = putils::split(category.c_str(), ':');
				kengine_assert_with_message(r, key_value.size() == 2, "--log_category_levels should be formatted as '--log_category_levels=first_category:log,second_category:verbose,third_category:error'");

				const auto & key = key_value[0];
				const auto value = putils::parse<log_severity>(key_value[1]);
				command_line_severity->category_severities[key] = value;
			}
		}

		kengine_log(r, very_verbose, "log_helper", "Setting minimum log severity");
		set_minimum_log_severity(r, command_line_severity->global_severity);
		for (const auto & [category, severity] : command_line_severity->category_severities)
			set_minimum_log_severity(r, severity);

		kengine_logf(r, very_verbose, "log_helper", "Parsed %s", putils::reflection::to_json(*command_line_severity).dump(4).c_str());
		return *command_line_severity;
	}

	void log(const entt::registry & r, log_severity severity, const char * category, const char * message) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (severity < max_log_severity)
			return;

		const kengine::log_event event{
			.severity = severity,
			.category = category,
			.message = message
		};

		for (const auto & [e, log] : r.view<functions::log>().each())
			log(event);
	}
}