#pragma once

// putils
#include "putils/string.hpp"
#include "putils/reflection_helpers/reflectible_formatter.hpp"

// kengine
#include "kengine/core/helpers/entt_formatter.hpp"
#include "kengine/core/log/helpers/log.hpp"

#ifndef KENGINE_LOG_MAX_SEVERITY
#define KENGINE_LOG_MAX_SEVERITY all
#endif

#ifdef KENGINE_NO_LOG
#define kengine_log(...) (void)0
#define kengine_logf(...) (void)0
#else
#define kengine_log(registry, verbosity, category, message) \
	do { \
		if constexpr (kengine::core::log::severity::verbosity >= kengine::core::log::severity::KENGINE_LOG_MAX_SEVERITY) \
			kengine::core::log::log(registry, kengine::core::log::severity::verbosity, category, message); \
	} while (false)
#define kengine_logf(registry, severity, category, format, ...) kengine_log(registry, severity, category, putils::string<1024>(format, __VA_ARGS__).c_str())
#endif
