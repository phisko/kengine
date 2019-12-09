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

#include "TestAmbientOcclusionGenerator.h"

#include "PolyVox/AmbientOcclusionCalculator.h"
#include "PolyVox/FilePager.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/RawVolume.h"

#include <QtTest>

using namespace PolyVox;

class IsVoxelTransparent
{
public:
	bool operator()(uint8_t voxel) const
	{
		return voxel == 0;
	}
};

bool isVoxelTransparentFunction(uint8_t voxel)
{
	return voxel == 0;
}

void TestAmbientOcclusionGenerator::testExecute()
{
	const int32_t g_uVolumeSideLength = 64;
	Region region(0, 0, 0, g_uVolumeSideLength - 1, g_uVolumeSideLength - 1, g_uVolumeSideLength - 1);

	//Create empty volume
	RawVolume<uint8_t> volData(region);

	// To test with a PagedVolume instead you can comment ou the above line and use these instead.
	// FilePager<uint8_t>* pFilePager = new FilePager<uint8_t>(".");
	// PagedVolume<uint8_t> volData(pFilePager, 256 * 1024 * 1024, 32);

	//Create two solid walls at opposite sides of the volume
	for (int32_t z = 0; z < g_uVolumeSideLength; z++)
	{
		if ((z < 20) || (z > g_uVolumeSideLength - 20))
		{
			for (int32_t y = 0; y < g_uVolumeSideLength; y++)
			{
				for (int32_t x = 0; x < g_uVolumeSideLength; x++)
				{
					volData.setVoxel(x, y, z, 1);
				}
			}
		}
	}

	//Create an array to store the result. Array can be smaller than the volume by an integer amount.
	const int32_t g_uArraySideLength = g_uVolumeSideLength / 2;
	Array<3, uint8_t> ambientOcclusionResult(g_uArraySideLength, g_uArraySideLength, g_uArraySideLength);

	// Calculate the ambient occlusion values
	IsVoxelTransparent isVoxelTransparent;
	QBENCHMARK{
		calculateAmbientOcclusion(&volData, &ambientOcclusionResult, region, 32.0f, 255, isVoxelTransparent);
	}

		//Check the results by sampling along a line though the centre of the volume. Because
		//of the two walls we added, samples in the middle are darker than those at the edge.
	QCOMPARE(static_cast<int>(ambientOcclusionResult(16, 0, 16)), 178);
	QCOMPARE(static_cast<int>(ambientOcclusionResult(16, 8, 16)), 109);
	QCOMPARE(static_cast<int>(ambientOcclusionResult(16, 16, 16)), 103);
	QCOMPARE(static_cast<int>(ambientOcclusionResult(16, 24, 16)), 123);
	QCOMPARE(static_cast<int>(ambientOcclusionResult(16, 31, 16)), 173);

	//Just run a quick test to make sure that it compiles when taking a function pointer
	calculateAmbientOcclusion(&volData, &ambientOcclusionResult, region, 32.0f, 8, &isVoxelTransparentFunction);

	//Also test it using a lambda
	//calculateAmbientOcclusion(&volData, &ambientOcclusionResult, volData.getEnclosingRegion(), 32.0f, 8, [](uint8_t voxel){return voxel == 0;});
}

QTEST_MAIN(TestAmbientOcclusionGenerator)
