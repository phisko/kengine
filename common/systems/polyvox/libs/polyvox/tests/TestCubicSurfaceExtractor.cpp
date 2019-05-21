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

#include "TestCubicSurfaceExtractor.h"

#include "PolyVox/Density.h"
#include "PolyVox/FilePager.h"
#include "PolyVox/Material.h"
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/CubicSurfaceExtractor.h"

#include <QtTest>

#include <random>

using namespace PolyVox;

template<typename _VoxelType>
class CustomIsQuadNeeded
{
public:
	typedef _VoxelType VoxelType;

	bool operator()(VoxelType back, VoxelType front, VoxelType& materialToUse)
	{
		// Not a useful test - it just does something different 
		// to the DefaultIsQuadNeeded so we can check it compiles.
		if ((back > 1) && (front <= 1))
		{
			materialToUse = static_cast<VoxelType>(back);
			return true;
		}
		else
		{
			return false;
		}
	}
};

// Runs the surface extractor for a given type. 
template <typename VolumeType>
void createAndFillVolumeWithNoise(VolumeType& volData, int32_t iVolumeSideLength, typename VolumeType::VoxelType minValue, typename VolumeType::VoxelType maxValue)
{
	// Set up a random number generator
	std::mt19937 rng;

	//Fill the volume with data
	for (int32_t z = 0; z < iVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < iVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < iVolumeSideLength; x++)
			{
				if (minValue == maxValue)
				{
					// In this case we are filling the whole volume with a single value.
					volData.setVoxel(x, y, z, minValue);
				}
				else
				{
					// Otherwise we write random voxel values between zero and the requested maximum
					// We can't use std distributions because they vary between platforms (breaking tests).
					int voxelValue = (rng() % (maxValue - minValue + 1)) + minValue; // +1 for inclusive bounds

					volData.setVoxel(x, y, z, static_cast<typename VolumeType::VoxelType>(voxelValue));
				}
			}
		}
	}
}

// Runs the surface extractor for a given type. 
template <typename VolumeType>
VolumeType* createAndFillVolumeRealistic(int32_t iVolumeSideLength)
{
	//Create empty volume
	FilePager<uint32_t>* filePager = new FilePager<uint32_t>();
	VolumeType* volData = new VolumeType(filePager);

	//Fill the volume with data
	for (int32_t z = 0; z < iVolumeSideLength; z++)
	{
		for (int32_t y = 0; y < iVolumeSideLength; y++)
		{
			for (int32_t x = 0; x < iVolumeSideLength; x++)
			{
				// Should create a checker board pattern stretched along z? This is 'realistic' in the sense
				// that it's not empty/random data, and should allow significant decimation to be performed.
				if ((x ^ y) & 0x01)
				{
					volData->setVoxel(x, y, z, 0);
				}
				else
				{
					volData->setVoxel(x, y, z, 1);
				}
			}
		}
	}

	return volData;
}

void TestCubicSurfaceExtractor::testBehaviour()
{
	int32_t iVolumeSideLength = 32;

	// Test with default mesh and contoller types.
	RawVolume<uint8_t> uint8Vol(Region(0, 0, 0, iVolumeSideLength - 1, iVolumeSideLength - 1, iVolumeSideLength - 1));
	createAndFillVolumeWithNoise(uint8Vol, 32, 0, 2);
	auto uint8Mesh = extractCubicMesh(&uint8Vol, uint8Vol.getEnclosingRegion());
	QCOMPARE(uint8Mesh.getNoOfVertices(), uint32_t(57544));
	QCOMPARE(uint8Mesh.getNoOfIndices(), uint32_t(215304));

	// Test with default mesh type but user-provided controller.
	RawVolume<int8_t> int8Vol(Region(0, 0, 0, iVolumeSideLength - 1, iVolumeSideLength - 1, iVolumeSideLength - 1));
	createAndFillVolumeWithNoise(int8Vol, 32, 0, 2);
	auto int8Mesh = extractCubicMesh(&int8Vol, int8Vol.getEnclosingRegion(), CustomIsQuadNeeded<int8_t>());
	QCOMPARE(int8Mesh.getNoOfVertices(), uint32_t(29106));
	QCOMPARE(int8Mesh.getNoOfIndices(), uint32_t(178566));

	// Test with default controller but user-provided mesh.
	RawVolume<uint32_t> uint32Vol(Region(0, 0, 0, iVolumeSideLength - 1, iVolumeSideLength - 1, iVolumeSideLength - 1));
	createAndFillVolumeWithNoise(uint32Vol, 32, 0, 2);
	Mesh< CubicVertex< uint32_t >, uint16_t > uint32Mesh;
	extractCubicMeshCustom(&uint32Vol, uint32Vol.getEnclosingRegion(), &uint32Mesh);
	QCOMPARE(uint32Mesh.getNoOfVertices(), uint16_t(57544));
	QCOMPARE(uint32Mesh.getNoOfIndices(), uint32_t(215304));

	// Test with both mesh and controller being provided by the user.
	RawVolume<int32_t> int32Vol(Region(0, 0, 0, iVolumeSideLength - 1, iVolumeSideLength - 1, iVolumeSideLength - 1));
	createAndFillVolumeWithNoise(int32Vol, 32, 0, 2);
	Mesh< CubicVertex< int32_t >, uint16_t > int32Mesh;
	extractCubicMeshCustom(&int32Vol, int32Vol.getEnclosingRegion(), &int32Mesh, CustomIsQuadNeeded<int32_t>());
	QCOMPARE(int32Mesh.getNoOfVertices(), uint16_t(29106));
	QCOMPARE(int32Mesh.getNoOfIndices(), uint32_t(178566));
}

void TestCubicSurfaceExtractor::testEmptyVolumePerformance()
{
	FilePager<uint32_t>* filePager = new FilePager<uint32_t>();
	PagedVolume<uint32_t> emptyVol(filePager);
	createAndFillVolumeWithNoise(emptyVol, 128, 0, 0);
	Mesh< CubicVertex< uint32_t >, uint16_t > emptyMesh;
	QBENCHMARK{ extractCubicMeshCustom(&emptyVol, Region(32, 32, 32, 63, 63, 63), &emptyMesh); }
	QCOMPARE(emptyMesh.getNoOfVertices(), uint16_t(0));
}

void TestCubicSurfaceExtractor::testRealisticVolumePerformance()
{
	auto realisticVol = createAndFillVolumeRealistic< PagedVolume<uint32_t> >(128);
	Mesh< CubicVertex< uint32_t >, uint16_t > realisticMesh;
	QBENCHMARK{ extractCubicMeshCustom(realisticVol, Region(32, 32, 32, 63, 63, 63), &realisticMesh); }
	QCOMPARE(realisticMesh.getNoOfVertices(), uint16_t(2176));
}

void TestCubicSurfaceExtractor::testNoiseVolumePerformance()
{
	FilePager<uint32_t>* filePager = new FilePager<uint32_t>();
	PagedVolume<uint32_t> noiseVol(filePager);
	createAndFillVolumeWithNoise(noiseVol, 128, 0, 2);
	Mesh< CubicVertex< uint32_t >, uint16_t > noiseMesh;
	QBENCHMARK{ extractCubicMeshCustom(&noiseVol, Region(32, 32, 32, 63, 63, 63), &noiseMesh); }
	QCOMPARE(noiseMesh.getNoOfVertices(), uint16_t(57905));
}

QTEST_MAIN(TestCubicSurfaceExtractor)
