/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 Matthew Williams and David Williams
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

#ifndef __PolyVox_TestVolume_H__
#define __PolyVox_TestVolume_H__

#include "PolyVox/FilePager.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/Region.h"

#include <QObject>

class TestVolume: public QObject
{
	Q_OBJECT

public:
	TestVolume();
	~TestVolume();
	
private slots:
	void testRawVolumeDirectAccessAllInternalForwards();
	void testRawVolumeSamplersAllInternalForwards();
	void testRawVolumeDirectAccessWithExternalForwards();
	void testRawVolumeSamplersWithExternalForwards();
	void testRawVolumeDirectAccessAllInternalBackwards();
	void testRawVolumeSamplersAllInternalBackwards();
	void testRawVolumeDirectAccessWithExternalBackwards();
	void testRawVolumeSamplersWithExternalBackwards();

	void testPagedVolumeDirectAccessAllInternalForwards();
	void testPagedVolumeSamplersAllInternalForwards();
	void testPagedVolumeDirectAccessWithExternalForwards();
	void testPagedVolumeSamplersWithExternalForwards();
	void testPagedVolumeDirectAccessAllInternalBackwards();
	void testPagedVolumeSamplersAllInternalBackwards();
	void testPagedVolumeDirectAccessWithExternalBackwards();
	void testPagedVolumeSamplersWithExternalBackwards();

	void testRawVolumeDirectRandomAccess();
	void testPagedVolumeDirectRandomAccess();

	void testPagedVolumeChunkLocalAccess();
	void testPagedVolumeChunkRandomAccess();

private:
	int32_t testPagedVolumeChunkAccess(uint16_t localityMask);

	static const uint16_t m_uChunkSideLength = 32;

	PolyVox::Region m_regVolume;
	PolyVox::Region m_regInternal;
	PolyVox::Region m_regExternal;
	PolyVox::FilePager<int32_t>* m_pFilePager;
	PolyVox::FilePager<int32_t>* m_pFilePagerHighMem;

	PolyVox::RawVolume<int32_t>* m_pRawVolume;
	PolyVox::PagedVolume<int32_t>* m_pPagedVolume;
	PolyVox::PagedVolume<int32_t>* m_pPagedVolumeHighMem;

	PolyVox::PagedVolume<uint32_t>::Chunk* m_pPagedVolumeChunk;
};

#endif
