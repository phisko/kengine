/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 David Williams and Matthew Williams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#ifndef __PolyVox_Logging_H__
#define __PolyVox_Logging_H__

#include <iostream>
#include <string>

// We expose the logger class to the user so that they can provide custom implementations
// to redirect PolyVox's log messages. However, it is not expected that user code will make 
// use of PolyVox's logging macros s these are part of the private implementation.
namespace PolyVox
{
	class Logger
	{
	public:
		Logger() {};
		virtual ~Logger() {};

		virtual void logTraceMessage(const std::string& message) = 0;
		virtual void logDebugMessage(const std::string& message) = 0;
		virtual void logInfoMessage(const std::string& message) = 0;
		virtual void logWarningMessage(const std::string& message) = 0;
		virtual void logErrorMessage(const std::string& message) = 0;
		virtual void logFatalMessage(const std::string& message) = 0;
	};

	class DefaultLogger : public Logger
	{
	public:
		DefaultLogger() : Logger() {}
		virtual ~DefaultLogger() {}

		// Appending the 'std::endl' forces the stream to be flushed.
		void logTraceMessage(const std::string& /*message*/) {  }
		void logDebugMessage(const std::string& /*message*/) {  }
		void logInfoMessage(const std::string& message) { std::cout << message << std::endl; }
		void logWarningMessage(const std::string& message) { std::cerr << message << std::endl; }
		void logErrorMessage(const std::string& message) { std::cerr << message << std::endl; }
		void logFatalMessage(const std::string& message) { std::cerr << message << std::endl; }
	};

	inline Logger*& getLoggerInstance()
	{
		static Logger* s_pLogger = new DefaultLogger;
		return s_pLogger;
	}

	inline void setLoggerInstance(Logger* pLogger)
	{
		getLoggerInstance() = pLogger;
	}
}

#endif //__PolyVox_Logging_H__
