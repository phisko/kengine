#pragma once

#ifdef KENGINE_NDEBUG
#define kengine_assert(x) (void)0
#define kengine_assert_with_message(x, ...) (void)0
#define kengine_assert_failed(...) (void)0
#define kengine_debug_break (void)0
#else

// stl
#include <functional>
#include <string>

// windows
#ifdef WIN32
#include "windows.h"
#include "dbghelp.h"
#define kengine_debug_break DebugBreak()
#else
#define kengine_debug_break __builtin_trap()
#endif

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/concatenate.hpp"

#define kengine_assert_failed(r, ...) \
	do { \
		const bool should_break = kengine::assert_helper::assert_failed(r, __FILE__, __LINE__, putils::concatenate(__VA_ARGS__)); \
		if (should_break && kengine::assert_helper::is_debugger_present()) \
			kengine_debug_break; \
	} while (false)

#define kengine_assert_with_message(r, x, ...) \
	do { \
		if (!(x)) \
			kengine_assert_failed(r, __VA_ARGS__); \
	} while (false)

#define kengine_assert(r, x) \
	kengine_assert_with_message(r, x, #x)

namespace kengine {
	namespace assert_helper {
		KENGINE_CORE_EXPORT extern std::function<bool(const entt::registry & r, const char * file, int line, const std::string & expr)> assert_handler;

		KENGINE_CORE_EXPORT bool assert_failed(const entt::registry & r, const char * file, int line, const std::string & expr) noexcept;
		KENGINE_CORE_EXPORT bool is_debugger_present() noexcept;
	}
}
#endif