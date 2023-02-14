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

#include "TestLowPassFilter.h"

#include "PolyVox/Density.h"
#include "PolyVox/LowPassFilter.h"
#include "PolyVox/RawVolume.h"

#include <QtTest>

using namespace PolyVox;

void TestLowPassFilter::testExecute()
{
	const int32_t g_uVolumeSideLength = 8;

	Region reg(Vector3DInt32(0, 0, 0), Vector3DInt32(g_uVolumeSideLength - 1, g_uVolumeSideLength - 1, g_uVolumeSideLength - 1));

	//Create empty volume
	RawVolume<Density8> volData(reg);

	//Create two solid walls at opposite sides of the volume
	for (int32_t z = 0; z < g_uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < g_uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < g_uVolumeSideLength; x++)
			{
				if (x % 2 == 0)
				{
					Density8 voxel(32);
					volData.setVoxel(x, y, z, voxel);
				}
			}
		}
	}

	RawVolume<Density8> resultVolume(reg);

	LowPassFilter< RawVolume<Density8>, RawVolume<Density8>, Density16 > lowPassfilter(&volData, reg, &resultVolume, reg, 3);

	//Test the normal implementation
	QBENCHMARK{
		lowPassfilter.execute();
	}
	QCOMPARE(resultVolume.getVoxel(0, 0, 0), Density8(4));
	QCOMPARE(resultVolume.getVoxel(1, 1, 1), Density8(21));
	QCOMPARE(resultVolume.getVoxel(2, 2, 2), Density8(10));
	QCOMPARE(resultVolume.getVoxel(3, 3, 3), Density8(21));
	QCOMPARE(resultVolume.getVoxel(4, 4, 4), Density8(10));
	QCOMPARE(resultVolume.getVoxel(5, 5, 5), Density8(21));
	QCOMPARE(resultVolume.getVoxel(6, 6, 6), Density8(10));
	QCOMPARE(resultVolume.getVoxel(7, 7, 7), Density8(4));

	//Test the SAT implmentation
	QBENCHMARK{
		lowPassfilter.executeSAT();
	}
	QCOMPARE(resultVolume.getVoxel(0, 0, 0), Density8(4));
	QCOMPARE(resultVolume.getVoxel(1, 1, 1), Density8(21));
	QCOMPARE(resultVolume.getVoxel(2, 2, 2), Density8(10));
	QCOMPARE(resultVolume.getVoxel(3, 3, 3), Density8(21));
	QCOMPARE(resultVolume.getVoxel(4, 4, 4), Density8(10));
	QCOMPARE(resultVolume.getVoxel(5, 5, 5), Density8(21));
	QCOMPARE(resultVolume.getVoxel(6, 6, 6), Density8(10));
	QCOMPARE(resultVolume.getVoxel(7, 7, 7), Density8(4));
}

QTEST_MAIN(TestLowPassFilter)
