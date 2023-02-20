#include "parse_command_line_severity.hpp"

// putils
#include "putils/command_line_arguments.hpp"
#include "putils/reflection_helpers/json_helper.hpp"
#include "putils/split.hpp"

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine core/log
#include "kengine/core/log/helpers/kengine_log.hpp"

// kengine command_line
#include "kengine/command_line/data/arguments.hpp"

namespace {
	struct options {
		kengine::core::log::severity log_level = kengine::core::log::severity::log;
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

namespace kengine::core::log {
	severity_control parse_command_line_severity(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "log_helper", "Parsing command-line severity");

		static std::optional<severity_control> command_line_severity;
		if (command_line_severity != std::nullopt) {
			kengine_logf(r, very_verbose, "log_helper", "Found pre-parsed %s", putils::reflection::to_json(*command_line_severity).dump(4).c_str());
			return *command_line_severity;
		}

		command_line_severity = severity_control{};

		for (const auto & [e, command_line] : r.view<command_line::arguments>().each()) {
			const auto opts = putils::parse_arguments<options>(command_line.args);
			kengine_logf(r, very_verbose, "log_helper", "Found %s in [%u]", putils::reflection::to_json(opts).dump(4).c_str(), e);

			command_line_severity->global_severity = opts.log_level;
			const auto categories = putils::split(opts.log_category_levels.c_str(), ',');
			for (const auto & category : categories) {
				const auto key_value = putils::split(category.c_str(), ':');
				if (key_value.size() != 2) {
					kengine_log(r, error, "core_log", "--log_category_levels should be formatted as '--log_category_levels=first_category:log,second_category:verbose,third_category:error'");
					continue;
				}

				const auto & key = key_value[0];
				const auto value = putils::parse<severity>(key_value[1]);
				command_line_severity->category_severities[key] = value;
			}
		}

		kengine_logf(r, very_verbose, "log_helper", "Parsed %s", putils::reflection::to_json(*command_line_severity).dump(4).c_str());
		return *command_line_severity;
	}
}