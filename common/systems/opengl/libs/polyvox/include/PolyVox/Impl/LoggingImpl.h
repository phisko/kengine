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

#ifndef __PolyVox_LoggingImpl_H__
#define __PolyVox_LoggingImpl_H__

#include "../Config.h"
#include "../Logging.h"

#include <sstream>

namespace PolyVox
{
	namespace Impl
	{
		// Used for building the log messages - convert a list of variables into a string.
		// Based on second approach here: http://stackoverflow.com/a/25386444/2337254
		template< typename ... Args >
		std::string argListToString(const Args& ... args)
		{
			std::ostringstream oss;
			int a[] = { 0, ((void)(oss << args), 0) ... };
			(void)a; // POLYVOX_UNUSED() doesn't seem to work here?
			return oss.str();
		}

		// Log trace message
		template< typename ... Args >
		void logTraceMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			getLoggerInstance()->logTraceMessage(message);
		}

		template< typename ... Args >
		void logTraceMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logTraceMessage(messageArgs...); }
		}

		// Log debug message
		template< typename ... Args >
		void logDebugMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			getLoggerInstance()->logDebugMessage(message);
		}

		template< typename ... Args >
		void logDebugMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logDebugMessage(messageArgs...); }
		}

		// Log info message
		template< typename ... Args >
		void logInfoMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			getLoggerInstance()->logInfoMessage(message);
		}

		template< typename ... Args >
		void logInfoMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logInfoMessage(messageArgs...); }
		}

		// Log warning message
		template< typename ... Args >
		void logWarningMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			getLoggerInstance()->logWarningMessage(message);
		}

		template< typename ... Args >
		void logWarningMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logWarningMessage(messageArgs...); }
		}

		// Log error message
		template< typename ... Args >
		void logErrorMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			getLoggerInstance()->logErrorMessage(message);
		}

		template< typename ... Args >
		void logErrorMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logErrorMessage(messageArgs...); }
		}

		// Log fatal message
		template< typename ... Args >
		void logFatalMessage(Args const& ... messageArgs)
		{
			std::string message = argListToString(messageArgs...);
			getLoggerInstance()->logFatalMessage(message);
		}

		template< typename ... Args >
		void logFatalMessageIf(bool condition, Args const& ... messageArgs)
		{
			if (condition) { logFatalMessage(messageArgs...); }
		}
	}
}

#ifdef POLYVOX_LOG_TRACE_ENABLED
#define POLYVOX_LOG_TRACE(...) PolyVox::Impl::logTraceMessage(__VA_ARGS__)
	#define POLYVOX_LOG_TRACE_IF(condition, ...) PolyVox::Impl::logTraceMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_TRACE(...)
	#define POLYVOX_LOG_TRACE_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_DEBUG_ENABLED
#define POLYVOX_LOG_DEBUG(...) PolyVox::Impl::logDebugMessage(__VA_ARGS__)
#define POLYVOX_LOG_DEBUG_IF(condition, ...) PolyVox::Impl::logDebugMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_DEBUG(...)
	#define POLYVOX_LOG_DEBUG_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_INFO_ENABLED
#define POLYVOX_LOG_INFO(...) PolyVox::Impl::logInfoMessage(__VA_ARGS__)
#define POLYVOX_LOG_INFO_IF(condition, ...) PolyVox::Impl::logInfoMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_INFO(...)
	#define POLYVOX_LOG_INFO_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_WARNING_ENABLED
#define POLYVOX_LOG_WARNING(...) PolyVox::Impl::logWarningMessage(__VA_ARGS__)
#define POLYVOX_LOG_WARNING_IF(condition, ...) PolyVox::Impl::logWarningMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_WARNING(...)
	#define POLYVOX_LOG_WARNING_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_ERROR_ENABLED
#define POLYVOX_LOG_ERROR(...) PolyVox::Impl::logErrorMessage(__VA_ARGS__)
#define POLYVOX_LOG_ERROR_IF(condition, ...) PolyVox::Impl::logErrorMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_ERROR(...)
	#define POLYVOX_LOG_ERROR_IF(condition, ...)
#endif

#ifdef POLYVOX_LOG_FATAL_ENABLED
#define POLYVOX_LOG_FATAL(...) PolyVox::Impl::logFatalMessage(__VA_ARGS__)
#define POLYVOX_LOG_FATAL_IF(condition, ...) PolyVox::Impl::logFatalMessageIf(condition, __VA_ARGS__)	
#else
	#define POLYVOX_LOG_FATAL(...)
	#define POLYVOX_LOG_FATAL_IF(condition, ...)
#endif

#endif //__PolyVox_LoggingImpl_H__
