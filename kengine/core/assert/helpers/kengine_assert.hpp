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
#include "putils/string.hpp"

// kengine
#include "kengine/core/assert/helpers/assert_failed.hpp"
#include "kengine/core/assert/helpers/is_debugger_present.hpp"

#define kengine_assert_failed(r, ...) \
	do { \
		const bool should_break = kengine::core::assert::assert_failed(r, __FILE__, __LINE__, putils::string<1024>(__VA_ARGS__).c_str()); \
		if (should_break && kengine::core::assert::is_debugger_present()) \
			kengine_debug_break; \
	} while (false)

#define kengine_assert_with_message(r, x, ...) \
	do { \
		if (!(x)) \
			kengine_assert_failed(r, __VA_ARGS__); \
	} while (false)

#define kengine_assert(r, x) \
	kengine_assert_with_message(r, x, #x)

#endif