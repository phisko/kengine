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

#include "TestRaycast.h"

#include "PolyVox/Density.h"
#include "PolyVox/Raycast.h"
#include "PolyVox/RawVolume.h"

#include "PolyVox/Impl/RandomUnitVectors.h"

#include <QtTest>

using namespace PolyVox;

// This is the callback functor which is called by the raycast() function for every voxel it touches.
// It's primary purpose is to tell the raycast whether or not to continue (i.e. it tests whether the
// ray has hit a solid voxel). Because the instance of this class is passed to the raycast() function
// by reference we can also use it to encapsulate some state. We're testing this by counting the total
// number of voxels touched.
class RaycastTestFunctor
{
public:
	RaycastTestFunctor()
		:m_uVoxelsTouched(0)
		, m_bRayLeftVolume(false)
	{
	}

	bool operator()(const RawVolume<int8_t>::Sampler& sampler)
	{
		m_uVoxelsTouched++;

		// For this particular test we know that we are always starting a ray inside the volume,
		// so if it ever leaves the volume we know it can't go back in and so we can terminate early.
		// This optimisation is worthwhile because samplers get slow once outside the volume.
		if (!sampler.isCurrentPositionValid())
		{
			m_bRayLeftVolume = true;
			return false;
		}

		// We are in the volume, so decide whether to continue based on the voxel value.
		return sampler.getVoxel() <= 0;
	}

	uint32_t m_uVoxelsTouched;
	bool m_bRayLeftVolume;
};

void TestRaycast::testExecute()
{
	const int32_t uVolumeSideLength = 32;

	//Create a hollow volume, with solid sides on x and y but with open ends in z.
	RawVolume<int8_t> volData(Region(Vector3DInt32(0, 0, 0), Vector3DInt32(uVolumeSideLength - 1, uVolumeSideLength - 1, uVolumeSideLength - 1)));
	for (int32_t z = 0; z < uVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < uVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < uVolumeSideLength; x++)
			{
				if ((x == 0) || (x == uVolumeSideLength - 1) || (y == 0) || (y == uVolumeSideLength - 1))
				{
					volData.setVoxel(x, y, z, 100);
				}
				else
				{
					volData.setVoxel(x, y, z, -100);
				}
			}
		}
	}

	//Cast rays from the centre. Roughly 2/3 should escape.
	Vector3DFloat start(uVolumeSideLength / 2, uVolumeSideLength / 2, uVolumeSideLength / 2);

	// We could have counted the total number of hits in the same way as the total number of voxels
	// touched, but for demonstration and testing purposes we are making use of the raycast return value
	// and counting them seperatly in this variable.
	int hits = 0;
	uint32_t uTotalVoxelsTouched = 0;

	// Cast a large number of random rays
	for (int ct = 0; ct < 1000000; ct++)
	{
		RaycastTestFunctor raycastTestFunctor;
		RaycastResult result = raycastWithDirection(&volData, start, randomUnitVectors[ct % 1024] * 1000.0f, raycastTestFunctor);

		uTotalVoxelsTouched += raycastTestFunctor.m_uVoxelsTouched;

		// If the raycast completed then we know it did not hit anything.If it was interupted then it
		// probably hit something, unless we noted that the reason it was interupted was that it left the volume.
		if ((result == RaycastResults::Interupted) && (raycastTestFunctor.m_bRayLeftVolume == false))
		{
			hits++;
		}
	}

	// Check the number of hits.
	QCOMPARE(hits, 687494);

	// Check the total number of voxels touched
	QCOMPARE(uTotalVoxelsTouched, static_cast<uint32_t>(29783248));
}

QTEST_MAIN(TestRaycast)
