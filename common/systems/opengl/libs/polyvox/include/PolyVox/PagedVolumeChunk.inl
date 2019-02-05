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

#include "Impl/Morton.h"
#include "Impl/Utility.h"

namespace PolyVox
{
	template <typename VoxelType>
	PagedVolume<VoxelType>::Chunk::Chunk(Vector3DInt32 v3dPosition, uint16_t uSideLength, Pager* pPager)
		:m_uChunkLastAccessed(0)
		, m_bDataModified(true)
		, m_tData(0)
		, m_uSideLength(0)
		, m_uSideLengthPower(0)
		, m_pPager(pPager)
		, m_v3dChunkSpacePosition(v3dPosition)
	{
		POLYVOX_ASSERT(m_pPager, "No valid pager supplied to chunk constructor.");
		POLYVOX_ASSERT(uSideLength <= 256, "Chunk side length cannot be greater than 256.");

		// Compute the side length               
		m_uSideLength = uSideLength;
		m_uSideLengthPower = logBase2(uSideLength);

		// Allocate the data
		const uint32_t uNoOfVoxels = m_uSideLength * m_uSideLength * m_uSideLength;
		m_tData = new VoxelType[uNoOfVoxels];

		// Pass the chunk to the Pager to give it a chance to initialise it with any data
		// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
		Vector3DInt32 v3dLower = m_v3dChunkSpacePosition * static_cast<int32_t>(m_uSideLength);
		Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uSideLength - 1, m_uSideLength - 1, m_uSideLength - 1);
		Region reg(v3dLower, v3dUpper);

		// A valid pager is normally present - this check is mostly to ease unit testing.
		if (m_pPager)
		{
			// Page the data in
			m_pPager->pageIn(reg, this);
		}

		// We'll use this later to decide if data needs to be paged out again.
		m_bDataModified = false;
	}

	template <typename VoxelType>
	PagedVolume<VoxelType>::Chunk::~Chunk()
	{
		if (m_bDataModified && m_pPager)
		{
			// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
			Vector3DInt32 v3dLower = m_v3dChunkSpacePosition * static_cast<int32_t>(m_uSideLength);
			Vector3DInt32 v3dUpper = v3dLower + Vector3DInt32(m_uSideLength - 1, m_uSideLength - 1, m_uSideLength - 1);

			// Page the data out
			m_pPager->pageOut(Region(v3dLower, v3dUpper), this);
		}

		delete[] m_tData;
		m_tData = 0;
	}

	template <typename VoxelType>
	VoxelType* PagedVolume<VoxelType>::Chunk::getData(void) const
	{
		return m_tData;
	}

	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::Chunk::getDataSizeInBytes(void) const
	{
		return m_uSideLength * m_uSideLength * m_uSideLength * sizeof(VoxelType);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Chunk::getVoxel(uint32_t uXPos, uint32_t uYPos, uint32_t uZPos) const
	{
		// This code is not usually expected to be called by the user, with the exception of when implementing paging 
		// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(m_tData, "No uncompressed data - chunk must be decompressed before accessing voxels.");

		uint32_t index = morton256_x[uXPos] | morton256_y[uYPos] | morton256_z[uZPos];

		return m_tData[index];
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Chunk::getVoxel(const Vector3DUint16& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Chunk::setVoxel(uint32_t uXPos, uint32_t uYPos, uint32_t uZPos, VoxelType tValue)
	{
		// This code is not usually expected to be called by the user, with the exception of when implementing paging 
		// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
		POLYVOX_ASSERT(uXPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uYPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(uZPos < m_uSideLength, "Supplied position is outside of the chunk");
		POLYVOX_ASSERT(m_tData, "No uncompressed data - chunk must be decompressed before accessing voxels.");

		uint32_t index = morton256_x[uXPos] | morton256_y[uYPos] | morton256_z[uZPos];

		m_tData[index] = tValue;

		this->m_bDataModified = true;
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Chunk::setVoxel(const Vector3DUint16& v3dPos, VoxelType tValue)
	{
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(void)
	{
		// Call through to the static version
		return calculateSizeInBytes(m_uSideLength);
	}

	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(uint32_t uSideLength)
	{
		// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
		// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
		uint32_t uSizeInBytes = uSideLength * uSideLength * uSideLength * sizeof(VoxelType);
		return  uSizeInBytes;
	}

	// This convienience function exists for historical reasons. Chunks used to store their data in 'linear' order but now we
	// use Morton encoding. Users who still have data in linear order (on disk, in databases, etc) will need to call this function
	// if they load the data in by memcpy()ing it via the raw pointer. On the other hand, if they set the data using setVoxel()
	// then the ordering is automatically handled correctly. 
	template <typename VoxelType>
	void PagedVolume<VoxelType>::Chunk::changeLinearOrderingToMorton(void)
	{
		VoxelType* pTempBuffer = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];

		// We should prehaps restructure this loop. From: https://fgiesen.wordpress.com/2011/01/17/texture-tiling-and-swizzling/
		//
		// "There's two basic ways to structure the actual swizzling: either you go through the (linear) source image in linear order, 
		// writing in (somewhat) random order, or you iterate over the output data, picking the right source pixel for each target
		// location. The former is more natural, especially when updating subrects of the destination texture (the source pixels still
		// consist of one linear sequence of bytes per line; the pattern of destination addresses written is considerably more
		// complicated), but the latter is usually much faster, especially if the source image data is in cached memory while the output
		// data resides in non-cached write-combined memory where non-sequential writes are expensive."
		//
		// This is something to consider if profiling identifies it as a hotspot.
		for (uint16_t z = 0; z < m_uSideLength; z++)
		{
			for (uint16_t y = 0; y < m_uSideLength; y++)
			{
				for (uint16_t x = 0; x < m_uSideLength; x++)
				{
					uint32_t uLinearIndex = x + y * m_uSideLength + z * m_uSideLength * m_uSideLength;
					uint32_t uMortonIndex = morton256_x[x] | morton256_y[y] | morton256_z[z];
					pTempBuffer[uMortonIndex] = m_tData[uLinearIndex];
				}
			}
		}

		std::memcpy(m_tData, pTempBuffer, getDataSizeInBytes());

		delete[] pTempBuffer;
	}

	// Like the above function, this is provided fot easing backwards compatibility. In Cubiquity we have some
	// old databases which use linear ordering, and we need to continue to save such data in linear order.
	template <typename VoxelType>
	void PagedVolume<VoxelType>::Chunk::changeMortonOrderingToLinear(void)
	{
		VoxelType* pTempBuffer = new VoxelType[m_uSideLength * m_uSideLength * m_uSideLength];
		for (uint16_t z = 0; z < m_uSideLength; z++)
		{
			for (uint16_t y = 0; y < m_uSideLength; y++)
			{
				for (uint16_t x = 0; x < m_uSideLength; x++)
				{
					uint32_t uLinearIndex = x + y * m_uSideLength + z * m_uSideLength * m_uSideLength;
					uint32_t uMortonIndex = morton256_x[x] | morton256_y[y] | morton256_z[z];
					pTempBuffer[uLinearIndex] = m_tData[uMortonIndex];
				}
			}
		}

		std::memcpy(m_tData, pTempBuffer, getDataSizeInBytes());

		delete[] pTempBuffer;
	}
}
