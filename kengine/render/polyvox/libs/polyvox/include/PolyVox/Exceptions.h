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

#ifndef __PolyVox_Exceptions_H__
#define __PolyVox_Exceptions_H__

#include <stdexcept> // For base exception classes.
#include <string> // Exception constuctors take strings.

// These exceptions form part of the public API because client code may need to catch them.
// Note that our utility macros such as 'POLYVOX_THROW_IF' do not form part of the public API
// as they are only for our own internal use.
namespace PolyVox
{
	/// A general purpose exception to indicate that an operation cannot be peformed.
	class invalid_operation : public std::logic_error
	{
	public:
		explicit invalid_operation(const std::string& message)
			: logic_error(message.c_str()) {}

		explicit invalid_operation(const char *message)
			: logic_error(message) {}
	};

	/// Thrown to indicate that a function is deliberatly not implmented. For example, perhaps you called a function
	/// in a base class whereas you are supposed to use a derived class which implements the function, or perhaps the
	/// function is not defined for a particular template parameter. It may be that the function is required to
	/// compile sucessfully but it should not be called.
	class not_implemented : public std::logic_error
	{
	public:
		explicit not_implemented(const std::string& message)
			: logic_error(message.c_str()) {}

		explicit not_implemented(const char *message)
			: logic_error(message) {}
	};
}

#endif //__PolyVox_Exceptions_H__
