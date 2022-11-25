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

#ifndef __PolyVox_Timer_H__
#define __PolyVox_Timer_H__

#include <chrono>
#include <cstdint>

namespace PolyVox
{
	class Timer
	{
	public:
		Timer(bool bAutoStart = true)
		{
			if (bAutoStart)
			{
				start();
			}
		}

		void start(void)
		{
			m_start = clock::now();
		}

		float elapsedTimeInSeconds(void)
		{
			std::chrono::duration<float> elapsed_seconds = clock::now() - m_start;
			return elapsed_seconds.count();
		}

		float elapsedTimeInMilliSeconds(void)
		{
			std::chrono::duration<float, std::milli> elapsed_milliseconds = clock::now() - m_start;
			return elapsed_milliseconds.count();
		}

		float elapsedTimeInMicroSeconds(void)
		{
			std::chrono::duration<float, std::micro> elapsed_microseconds = clock::now() - m_start;
			return elapsed_microseconds.count();
		}

	private:
		typedef std::chrono::system_clock clock;
		std::chrono::time_point<clock> m_start;
	};
}

#endif //__PolyVox_Timer_H__