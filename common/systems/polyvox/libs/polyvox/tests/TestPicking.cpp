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

#include "TestPicking.h"

#include "PolyVox/Picking.h"
#include "PolyVox/RawVolume.h"

#include <QtTest>

using namespace PolyVox;

void TestPicking::testExecute()
{
	const int32_t uVolumeSideLength = 32;

	RawVolume<int8_t> volData(Region(Vector3DInt32(0, 0, 0), Vector3DInt32(uVolumeSideLength - 1, uVolumeSideLength - 1, uVolumeSideLength - 1)));
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				if ((x > uVolumeSideLength / 2)) //x > 16 is filled
				{
					volData.setVoxel(x, y, z, 100);
				}
				else
				{
					volData.setVoxel(x, y, z, 0);
				}
			}
		}
	}

	const int8_t emptyVoxelExample = 0; //A voxel value of zero will represent empty space.	
	PickResult resultHit = pickVoxel(&volData, Vector3DFloat(0, uVolumeSideLength / 2, uVolumeSideLength / 2), Vector3DFloat(uVolumeSideLength, 0, 0), emptyVoxelExample);

	QCOMPARE(resultHit.didHit, true);
	QCOMPARE(resultHit.hitVoxel, Vector3DInt32((uVolumeSideLength / 2) + 1, uVolumeSideLength / 2, uVolumeSideLength / 2));
	QCOMPARE(resultHit.previousVoxel, Vector3DInt32((uVolumeSideLength / 2), uVolumeSideLength / 2, uVolumeSideLength / 2));

	PickResult resultMiss = pickVoxel(&volData, Vector3DFloat(0, uVolumeSideLength / 2, uVolumeSideLength / 2), Vector3DFloat(uVolumeSideLength / 2, uVolumeSideLength, uVolumeSideLength), emptyVoxelExample);

	QCOMPARE(resultMiss.didHit, false);
}

QTEST_MAIN(TestPicking)
