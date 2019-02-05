/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

// This file contains definitions for various macros, etc, which need to be different
// for each platform. It helps keep per-platform logic outside the rest of PolyVox.
#ifndef __PolyVox_PlatformDefinitions_H__
#define __PolyVox_PlatformDefinitions_H__

// An error message like the one below makes it much clearer to the user that they are using
// an unsupported compiler, rather than them wondering if they are using PolyVox incorrectly.
#if defined(_MSC_VER) && (_MSC_VER < 1800)
	#error "Your version of Visual Studio is too old to build PolyVox. You need at least version Visual Stusio 2013"
#endif

// Macros cannot contain #ifdefs, but some of our macros need to disable warnings and such warning supression is
// platform specific. But macros can contain other macros, so we create macros to control the warnings and use
// those instead. This set of warning supression macros can be extended to GCC/Clang when required.
#if defined(_MSC_VER)
	#define POLYVOX_MSC_WARNING_PUSH __pragma(warning(push))
	#define POLYVOX_DISABLE_MSC_WARNING(x) __pragma(warning(disable:x))
	#define POLYVOX_MSC_WARNING_POP __pragma(warning(pop))
#else
	#define POLYVOX_MSC_WARNING_PUSH
	#define POLYVOX_DISABLE_MSC_WARNING(x)
	#define POLYVOX_MSC_WARNING_POP
#endif

// Used to mark functions as deprecated prior to us removing them.
#if defined _WIN32 || defined __CYGWIN__
  #define POLYVOX_DEPRECATED __declspec(deprecated)
#else
  #define POLYVOX_DEPRECATED __attribute__((deprecated))
#endif

// The depreacated macro (and possibly others?) cause confusion for SWIG, so we undefine them if SWIG is running.
#if defined SWIG
  //Do nothing in this case
#else
  #undef POLYVOX_DEPRECATED
  #define POLYVOX_DEPRECATED //Define it to nothing to avoid warnings
#endif

// Halts the application is the most elegant way possible (dropping into a debugger if we can).
#if defined(_MSC_VER)
	// In Visual Studio we can use this function to go into the debugger.
	#define POLYVOX_HALT() __debugbreak()
#else
	// On other platforms we just halt by forcing a crash.
	// Hopefully this puts us in the debugger if one is running
	#if defined(__linux__) || defined(__APPLE__)
		#define POLYVOX_HALT() raise(SIGTRAP)
	#else
		#define POLYVOX_HALT() *((unsigned int*)0) = 0xDEAD
	#endif
#endif

// Used to prevent the compiler complaining about unused varuables, particularly useful when
// e.g. asserts are disabled and the parameter it was checking isn't used anywhere else.
// Note that this implementation doesn't seem to work everywhere, for some reason I have
// seen it give compile errors when combined with variadic template functions (to be confirmed)?
// Implementation from here: http://stackoverflow.com/a/4851173/2337254
#define POLYVOX_UNUSED(x) do { (void)sizeof(x); } while(0)

#endif //__PolyVox_PlatformDefinitions_H__
