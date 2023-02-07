#include "log_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// putils
#include "putils/command_line_arguments.hpp"

// kengine data
#include "kengine/data/command_line.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace {
	static kengine::log_severity max_log_severity = kengine::log_severity::none;

	struct options {
		kengine::log_severity log_level = kengine::log_severity::log;
	};
}

#define refltype options
putils_reflection_info {
	putils_reflection_custom_class_name(log);
	putils_reflection_attributes(
		putils_reflection_attribute(log_level)
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

	log_severity parse_command_line_severity(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "log_helper", "Parsing command-line severity");

		static std::optional<log_severity> command_line_severity;
		if (command_line_severity != std::nullopt) {
			kengine_logf(r, very_verbose, "log_helper", "Found pre-parsed %s", severity_to_string(*command_line_severity).c_str());
			return *command_line_severity;
		}

		log_severity result = log_severity::log;
		for (const auto & [e, command_line] : r.view<data::command_line>().each()) {
			const auto opts = putils::parse_arguments<options>(command_line.arguments);
			result = opts.log_level;
			kengine_logf(r, very_verbose, "log_helper", "Found %s in [%zu]", std::string(magic_enum::enum_names<log_severity>()[int(result)]).c_str(), e);
		}

		kengine_log(r, very_verbose, "log_helper", "Setting minimum log severity");
		set_minimum_log_severity(r, result);

		command_line_severity = result;
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