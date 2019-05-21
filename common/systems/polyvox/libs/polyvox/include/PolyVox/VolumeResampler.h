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

#ifndef __PolyVox_VolumeResampler_H__
#define __PolyVox_VolumeResampler_H__

#include "Region.h"

namespace PolyVox
{
	/**
	 * This class can be used to copy data from one volume to another, possibly while
	 * resizing it. It has not been heavily used an may or may not work as expected.
	 */
	template< typename SrcVolumeType, typename DstVolumeType>
	class VolumeResampler
	{
	public:
		VolumeResampler(SrcVolumeType* pVolSrc, const Region& regSrc, DstVolumeType* pVolDst, const Region& regDst);

		void execute();

	private:
		void resampleSameSize();
		void resampleArbitrary();

		//Source data
		SrcVolumeType* m_pVolSrc;
		Region m_regSrc;

		//Destination data
		DstVolumeType* m_pVolDst;
		Region m_regDst;
	};

}//namespace PolyVox

#include "VolumeResampler.inl"

#endif

