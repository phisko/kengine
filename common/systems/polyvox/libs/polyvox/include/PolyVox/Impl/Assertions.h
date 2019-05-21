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

#ifndef __PolyVox_Assertions_H__
#define __PolyVox_Assertions_H__

#include "../Config.h"

#include "LoggingImpl.h" // Asserts can log when they fire.
#include "PlatformDefinitions.h"

// The code below implements a custom assert function called POLYVOX_ASSERT which has a number of advantages compared
// to the standard C/C++ assert(). It is inspired by http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
// which provides code under the MIT license.

#ifdef POLYVOX_ASSERTS_ENABLED

	#define POLYVOX_ASSERT(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		(http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do \
			{ \
			if (!(condition)) \
				{ \
				std::stringstream ss; \
				ss << "\n"; \
				ss << "    PolyVox Assertion Failed!"; \
				ss << "    ========================="; \
				ss << "    Condition: " << #condition; \
				ss << "    Message:   " << (message); \
				ss << "    Location:  " << "Line " << __LINE__ << " of " << __FILE__; \
				ss << "\n"; \
				PolyVox::getLoggerInstance()->logFatalMessage(ss.str()); \
				POLYVOX_HALT(); \
				} \
			} while(0) \
			POLYVOX_MSC_WARNING_POP

#else

	#define POLYVOX_ASSERT(condition, message) \
		/* We use the do...while(0) construct in our macros (for reasons see here: http://stackoverflow.com/a/154138) \
		but Visual Studio gives unhelpful 'conditional expression is constant' warnings. The recommended solution \
		(http://stackoverflow.com/a/1946485) is to disable these warnings. */ \
		POLYVOX_MSC_WARNING_PUSH \
		POLYVOX_DISABLE_MSC_WARNING(4127) \
		do { POLYVOX_UNUSED(condition); POLYVOX_UNUSED(message); } while(0) \
		POLYVOX_MSC_WARNING_POP

#endif

#endif //__PolyVox_Assertions_H__
