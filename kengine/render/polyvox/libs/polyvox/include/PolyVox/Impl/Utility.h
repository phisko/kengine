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

#ifndef __PolyVox_Utility_H__
#define __PolyVox_Utility_H__

#include "PlatformDefinitions.h"

#include <cstdint>

namespace PolyVox
{
	inline bool isPowerOf2(uint32_t uInput)
	{
		if (uInput == 0)
			return false;
		else
			return ((uInput & (uInput - 1)) == 0);
	}

	//Note: this function only works for inputs which are a power of two and not zero
	//If this is not the case then the output is undefined.
	inline uint8_t logBase2(uint32_t uInput)
	{
		//Release mode validation
		if (uInput == 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Cannot compute the log of zero.");
		}
		if (!isPowerOf2(uInput))
		{
			POLYVOX_THROW(std::invalid_argument, "Input must be a power of two in order to compute the log.");
		}

		uint32_t uResult = 0;
		while ((uInput >> uResult) != 0)
		{
			++uResult;
		}
		return static_cast<uint8_t>(uResult - 1);
	}

	// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	inline uint32_t upperPowerOfTwo(uint32_t v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	inline int32_t roundTowardsNegInf(float r)
	{
		return (r >= 0.0) ? static_cast<int32_t>(r) : static_cast<int32_t>(r - 1.0f);
	}

	inline int32_t roundToNearestInteger(float r)
	{
		return (r >= 0.0) ? static_cast<int32_t>(r + 0.5f) : static_cast<int32_t>(r - 0.5f);
	}

	template <typename Type>
	inline Type clamp(const Type& value, const Type& low, const Type& high)
	{
		return (std::min)(high, (std::max)(low, value));
	}
}

#endif
