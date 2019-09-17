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

#include "testvolume.h"

#include "PolyVox/FilePager.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/RawVolume.h"

#include <QtGlobal>
#include <QtTest>

#include <random>

using namespace PolyVox;

// This is used to compute a value from a list of integers. We use it to 
// make sure we get the expected result from a series of volume accesses.
inline int32_t cantorTupleFunction(int32_t previousResult, int32_t value)
{
	return ((previousResult + value) * (previousResult + value + 1) + value) / 2;
}

/*
 * Funtions for testing iteration in a forwards direction
 */

// We allow user provided offset in this function so we can test the case when all samples are inside a volume and also the case when some samples are outside.
// This is important because samplers are often slower when outside the volume as they have to fall back on directly accessing the volume data.
template <typename VolumeType>
int32_t testDirectAccessWithWrappingForwards(const VolumeType* volume, Region region)
{
	int32_t result = 0;

	for (int z = region.getLowerZ(); z <= region.getUpperZ(); z++)
	{
		for (int y = region.getLowerY(); y <= region.getUpperY(); y++)
		{
			for (int x = region.getLowerX(); x <= region.getUpperX(); x++)
			{
				//Three level loop now processes 27 voxel neighbourhood
				for (int innerZ = -1; innerZ <= 1; innerZ++)
				{
					for (int innerY = -1; innerY <= 1; innerY++)
					{
						for (int innerX = -1; innerX <= 1; innerX++)
						{
							result = cantorTupleFunction(result, volume->getVoxel(x + innerX, y + innerY, z + innerZ));
						}
					}
				}
				//End of inner loops
			}
		}
	}

	return result;
}

template <typename VolumeType>
int32_t testSamplersWithWrappingForwards(VolumeType* volume, Region region)
{
	int32_t result = 0;

	//Test the sampler move functions
	typename VolumeType::Sampler sampler(volume);

	for (int z = region.getLowerZ(); z <= region.getUpperZ(); z++)
	{
		for (int y = region.getLowerY(); y <= region.getUpperY(); y++)
		{
			sampler.setPosition(region.getLowerX(), y, z);
			for (int x = region.getLowerX(); x <= region.getUpperX(); x++)
			{
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1ny1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1ny1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1ny1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx0py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px0py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px0py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1py1nz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1py1nz());

				result = cantorTupleFunction(result, sampler.peekVoxel1nx1ny0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1ny0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1ny0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx0py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px0py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px0py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1py0pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1py0pz());

				result = cantorTupleFunction(result, sampler.peekVoxel1nx1ny1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1ny1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1ny1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx0py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px0py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px0py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1nx1py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel0px1py1pz());
				result = cantorTupleFunction(result, sampler.peekVoxel1px1py1pz());

				sampler.movePositiveX();
			}
		}
	}

	return result;
}

/*
 * Funtions for testing iteration in a backwards direction
 */

// We allow user provided offset in this function so we can test the case when all samples are inside a volume and also the case when some samples are outside.
// This is important because samplers are often slower when outside the volume as they have to fall back on directly accessing the volume data.
template <typename VolumeType>
int32_t testDirectAccessWithWrappingBackwards(const VolumeType* volume, Region region)
{
	int32_t result = 0;

	for (int z = region.getUpperZ(); z >= region.getLowerZ(); z--)
	{
		for (int y = region.getUpperY(); y >= region.getLowerY(); y--)
		{
			for (int x = region.getUpperX(); x >= region.getLowerX(); x--)
			{
				//Three level loop now processes 27 voxel neighbourhood
				for (int innerZ = -1; innerZ <= 1; innerZ++)
				{
					for (int innerY = -1; innerY <= 1; innerY++)
					{
						for (int innerX = -1; innerX <= 1; innerX++)
						{
							result = cantorTupleFunction(result, volume->getVoxel(x + innerX, y + innerY, z + innerZ));
						}
					}
				}
				//End of inner loops
			}
		}
	}

	return result;
}

template <typename VolumeType>
int32_t testSamplersWithWrappingBackwards(VolumeType* volume, Region region)
{
	int32_t result = 0;

	//Test the sampler move functions
	typename VolumeType::Sampler xSampler(volume);

	for (int z = region.getUpperZ(); z >= region.getLowerZ(); z--)
	{
		for (int y = region.getUpperY(); y >= region.getLowerY(); y--)
		{
			xSampler.setPosition(region.getUpperX(), y, z);
			for (int x = region.getUpperX(); x >= region.getLowerX(); x--)
			{
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py1nz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py1nz());

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py0pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py0pz());

				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1ny1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px0py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1nx1py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel0px1py1pz());
				result = cantorTupleFunction(result, xSampler.peekVoxel1px1py1pz());

				xSampler.moveNegativeX();
			}
		}
	}

	return result;
}

template <typename VolumeType>
int32_t testDirectRandomAccess(const VolumeType* volume)
{
	std::mt19937 rng;
	int32_t result = 0;

	for (uint32_t ct = 0; ct < 10000000; ct++)
	{
		uint32_t rand = rng();

		// Four random number between 0-255
		uint32_t part0 = static_cast<int32_t>(rand & 0xFF);
		int32_t part1 = static_cast<int32_t>((rand >> 8) & 0xFF);
		int32_t part2 = static_cast<int32_t>((rand >> 16) & 0xFF);
		int32_t part3 = static_cast<int32_t>((rand >> 24) & 0xFF);

		result = cantorTupleFunction(result, volume->getVoxel(part0, part1, part2));
		result = cantorTupleFunction(result, volume->getVoxel(part1, part2, part3));
		result = cantorTupleFunction(result, volume->getVoxel(part2, part3, part0));
		result = cantorTupleFunction(result, volume->getVoxel(part3, part0, part1));

	}

	return result;
}

TestVolume::TestVolume()
{
	m_regVolume = Region(-57, -31, 12, 64, 96, 131); // Deliberatly awkward size

	m_regInternal = m_regVolume;
	m_regInternal.shiftLowerCorner(4, 2, 2);
	m_regInternal.shiftUpperCorner(-3, -1, -2);

	m_regExternal = m_regVolume;
	m_regExternal.shiftLowerCorner(-1, -3, -2);
	m_regExternal.shiftUpperCorner(2, 5, 4);

	m_pFilePager = new FilePager<int32_t>(".");
	m_pFilePagerHighMem = new FilePager<int32_t>(".");

	//Create the volumes
	m_pRawVolume = new RawVolume<int32_t>(m_regVolume);
	m_pPagedVolume = new PagedVolume<int32_t>(m_pFilePager, 1 * 1024 * 1024, m_uChunkSideLength);
	m_pPagedVolumeHighMem = new PagedVolume<int32_t>(m_pFilePagerHighMem, 256 * 1024 * 1024, m_uChunkSideLength);

	//Fill the volume with some data
	for (int z = m_regVolume.getLowerZ(); z <= m_regVolume.getUpperZ(); z++)
	{
		for (int y = m_regVolume.getLowerY(); y <= m_regVolume.getUpperY(); y++)
		{
			for (int x = m_regVolume.getLowerX(); x <= m_regVolume.getUpperX(); x++)
			{
				int32_t value = x + y + z;
				m_pRawVolume->setVoxel(x, y, z, value);
				m_pPagedVolume->setVoxel(x, y, z, value);
				m_pPagedVolumeHighMem->setVoxel(x, y, z, value);
			}
		}
	}

	m_pPagedVolumeChunk = new PagedVolume<uint32_t>::Chunk(Vector3DInt32(10000, 10000, 10000), m_uChunkSideLength, nullptr);
	std::mt19937 rng;
	for (uint16_t z = 0; z < m_uChunkSideLength; z++)
	{
		for (uint16_t y = 0; y < m_uChunkSideLength; y++)
		{
			for (uint16_t x = 0; x < m_uChunkSideLength; x++)
			{
				m_pPagedVolumeChunk->setVoxel(x, y, z, static_cast<uint32_t>(rng()));
			}
		}
	}
}

TestVolume::~TestVolume()
{
	delete m_pPagedVolumeChunk;

	delete m_pRawVolume;
	delete m_pPagedVolume;

	delete m_pFilePager;
}

/*
 * RawVolume Tests
 */

void TestVolume::testRawVolumeDirectAccessAllInternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pRawVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testRawVolumeSamplersAllInternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pRawVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testRawVolumeDirectAccessWithExternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pRawVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(337227750));
}

void TestVolume::testRawVolumeSamplersWithExternalForwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pRawVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(337227750));
}

void TestVolume::testRawVolumeDirectAccessAllInternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pRawVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testRawVolumeSamplersAllInternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pRawVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testRawVolumeDirectAccessWithExternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pRawVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-993539594));
}

void TestVolume::testRawVolumeSamplersWithExternalBackwards()
{
	int32_t result = 0;

	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pRawVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-993539594));
}

/*
 * PagedVolume Tests
 */

void TestVolume::testPagedVolumeDirectAccessAllInternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pPagedVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testPagedVolumeSamplersAllInternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pPagedVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(1004598054));
}

void TestVolume::testPagedVolumeDirectAccessWithExternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingForwards(m_pPagedVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(337227750));
}

void TestVolume::testPagedVolumeSamplersWithExternalForwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingForwards(m_pPagedVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(337227750));
}

void TestVolume::testPagedVolumeDirectAccessAllInternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pPagedVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testPagedVolumeSamplersAllInternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pPagedVolume, m_regInternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-269366578));
}

void TestVolume::testPagedVolumeDirectAccessWithExternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectAccessWithWrappingBackwards(m_pPagedVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-993539594));
}

void TestVolume::testPagedVolumeSamplersWithExternalBackwards()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testSamplersWithWrappingBackwards(m_pPagedVolume, m_regExternal);
	}
	QCOMPARE(result, static_cast<int32_t>(-993539594));
}

/*
 * Random access tests
 */
void TestVolume::testRawVolumeDirectRandomAccess()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectRandomAccess(m_pRawVolume);
	}
	QCOMPARE(result, static_cast<int32_t>(171835633));
}

void TestVolume::testPagedVolumeDirectRandomAccess()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testDirectRandomAccess(m_pPagedVolumeHighMem);
	}
	QCOMPARE(result, static_cast<int32_t>(171835633));
}

int32_t TestVolume::testPagedVolumeChunkAccess(uint16_t localityMask)
{
	std::mt19937 rng;
	int32_t sum = 0;
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t z = 0;

	// Used to constrain the range, as '&=m_uChunkSideLengthMask' should be faster than '%=m_uChunkSideLength'.
	uint16_t m_uChunkSideLengthMask = m_uChunkSideLength - 1;

	for (int32_t ct = 0; ct < 10000000; ct++)
	{
		// Random number generation is relatively slow compared to retireving a voxel, so if we are not
		// careful the process of choosing which voxel to get can become slower than actually getting it.
		// Therefore we use the same random number multiple times by getting different bits from it.
		uint32_t rand = rng();

		// We have 32 random bits and we make 27 (3*9) calls to getVoxel(). This means we
		// stop using the lower bits before they all get set to zero from the right-shifting.
		// An odd number means we have an imbalance between the number of times we go forward vs. 
		// backwards, so overall we will drift around the volume even if locality is constrained.
		for (uint32_t i = 0; i < 3; i++)
		{
			x += rand & localityMask; // Move x forwardsby a small amount, limited by localityMask.
			x &= m_uChunkSideLengthMask; // Ensure it is within the valid range.
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z); // Get the voxel value and use it.
			rand >>= 1; // Shift the bits so we use different ones next time. 

			y -= rand & localityMask; // This one (and some others) are negative so sometimes we go backwards.
			y &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			z += rand & localityMask;
			z &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			x -= rand & localityMask;
			x &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			y += rand & localityMask;
			y &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			z -= rand & localityMask;
			z &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			x += rand & localityMask;
			x &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			y -= rand & localityMask;
			y &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;

			z += rand & localityMask;
			z &= m_uChunkSideLengthMask;
			sum += m_pPagedVolumeChunk->getVoxel(x, y, z);
			rand >>= 1;
		}
	}

	// It's important to use the voxel values, otherwise
	// the compiler optimizes out the calls to getVoxel().
	return sum;
}

void TestVolume::testPagedVolumeChunkLocalAccess()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testPagedVolumeChunkAccess(0x0003); // Small value for good locality
	}
	QCOMPARE(result, static_cast<int32_t>(-158083685));
}

void TestVolume::testPagedVolumeChunkRandomAccess()
{
	int32_t result = 0;
	QBENCHMARK
	{
		result = testPagedVolumeChunkAccess(0xFFFF); // Large value for poor locality (random access)
	}
	QCOMPARE(result, static_cast<int32_t>(71649197));
}

QTEST_MAIN(TestVolume)
