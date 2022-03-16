#pragma once

#ifdef _MSC_VER
# define kengine_assume(x) __assume(x)
#else
# define kengine_assume(x) (void)0
#endif

#ifdef KENGINE_NDEBUG
# define kengine_assert(x) (void)0
# define kengine_assert_with_message(x, ...) (void)0
# define kengine_assert_failed(...) (void)0
# define kengine_debug_break (void)0
#else
# include <string>

# ifdef WIN32
#  include "windows.h"
#  include "dbghelp.h"
#  define kengine_debug_break DebugBreak()
# else
#  define kengine_debub_break (void)0
# endif

#include "concatenate.hpp"

# define kengine_assert_failed(...) \
	do {\
		const bool shouldBreak = kengine::assertHelper::assertFailed(__FILE__, __LINE__, putils::concatenate(__VA_ARGS__)); \
		if (shouldBreak && kengine::assertHelper::isDebuggerPresent()) \
			kengine_debug_break; \
	} while(false)

# define kengine_assert_with_message(x, ...) \
	do {\
		if (!!(x)) \
			(void)0; \
		else \
			kengine_assert_failed(putils::concatenate(__VA_ARGS__)); \
		kengine_assume(!!(x)); \
	} while (false)

# define kengine_assert(x) \
	kengine_assert_with_message(x, #x)

namespace kengine {
	namespace assertHelper {
        extern std::function<bool(const char * file, int line, const std::string & expr)> assertHandler;

		bool assertFailed(const char * file, int line, const std::string & expr) noexcept;
		bool isDebuggerPresent() noexcept;
    }
}
#endif