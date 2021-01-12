#pragma once

#ifdef WIN32
# define kengine_assume(x) __assume(x)
#else
# define kengine_assume(x) (void)0
#endif

#ifdef KENGINE_NDEBUG
# define kengine_assert(x) (void)0
# define kengine_assert_with_message(x, message) (void)0
# define kengine_assert_failed(x) (void)0
# define kengine_debug_break (void)0
#else
# include <string>

# ifdef WIN32
#  define kengine_debug_break DebugBreak()
# else
#  define kengine_debub_break (void)0
# endif
# define kengine_assert_failed(message) \
	do {\
		kengine::assertHelper::assertFailed(__FILE__, __LINE__, message); \
		if (kengine::assertHelper::isDebuggerPresent()) \
			kengine_debug_break; \
	} while(false)
# define kengine_assert_with_message(x, message) \
	do {\
		if (!!(x)) \
			(void)0; \
		else \
			kengine_assert_failed(message); \
		kengine_assume(!!(x)); \
	} while (false)
# define kengine_assert(x) \
	kengine_assert_with_message(x, #x)

namespace kengine {
	namespace assertHelper {
		void assertFailed(const char * file, int line, const std::string & expr) noexcept;
		bool isDebuggerPresent() noexcept;
	}
}
#endif