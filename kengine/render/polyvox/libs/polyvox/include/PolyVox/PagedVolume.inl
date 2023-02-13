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

#include "Impl/ErrorHandling.h"

#include <algorithm>
#include <limits>

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// This constructor creates a volume with a fixed size which is specified as a parameter. By default this constructor will not enable paging but you can override this if desired. If you do wish to enable paging then you are required to provide the call back function (see the other PagedVolume constructor).
	/// \param pPager Called by PolyVox to load and unload data on demand.
	/// \param uTargetMemoryUsageInBytes The upper limit to how much memory this PagedVolume should aim to use.
	/// \param uChunkSideLength The size of the chunks making up the volume. Small chunks will compress/decompress faster, but there will also be more of them meaning voxel access could be slower.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::PagedVolume(Pager* pPager, uint32_t uTargetMemoryUsageInBytes, uint16_t uChunkSideLength)
		:BaseVolume<VoxelType>()
		, m_uChunkSideLength(uChunkSideLength)
		, m_pPager(pPager)
	{
			// Validation of parameters
			POLYVOX_THROW_IF(!pPager, std::invalid_argument, "You must provide a valid pager when constructing a PagedVolume");
			POLYVOX_THROW_IF(uTargetMemoryUsageInBytes < 1 * 1024 * 1024, std::invalid_argument, "Target memory usage is too small to be practical");
			POLYVOX_THROW_IF(m_uChunkSideLength == 0, std::invalid_argument, "Chunk side length cannot be zero.");
			POLYVOX_THROW_IF(m_uChunkSideLength > 256, std::invalid_argument, "Chunk size is too large to be practical.");
			POLYVOX_THROW_IF(!isPowerOf2(m_uChunkSideLength), std::invalid_argument, "Chunk side length must be a power of two.");

			// Used to perform multiplications and divisions by bit shifting.
			m_uChunkSideLengthPower = logBase2(m_uChunkSideLength);
			// Use to perform modulo by bit operations
			m_iChunkMask = m_uChunkSideLength - 1;

			// Calculate the number of chunks based on the memory limit and the size of each chunk.
			uint32_t uChunkSizeInBytes = PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength);
			m_uChunkCountLimit = uTargetMemoryUsageInBytes / uChunkSizeInBytes;

			// Enforce sensible limits on the number of chunks.
			const uint32_t uMinPracticalNoOfChunks = 32; // Enough to make sure a chunks and it's neighbours can be loaded, with a few to spare.
			const uint32_t uMaxPracticalNoOfChunks = uChunkArraySize / 2; // A hash table should only become half-full to avoid too many clashes.
			POLYVOX_LOG_WARNING_IF(m_uChunkCountLimit < uMinPracticalNoOfChunks, "Requested memory usage limit of ",
				uTargetMemoryUsageInBytes / (1024 * 1024), "Mb is too low and cannot be adhered to.");
			m_uChunkCountLimit = (std::max)(m_uChunkCountLimit, uMinPracticalNoOfChunks);
			m_uChunkCountLimit = (std::min)(m_uChunkCountLimit, uMaxPracticalNoOfChunks);

			// Inform the user about the chosen memory configuration.
			POLYVOX_LOG_DEBUG("Memory usage limit for volume now set to ", (m_uChunkCountLimit * uChunkSizeInBytes) / (1024 * 1024),
				"Mb (", m_uChunkCountLimit, " chunks of ", uChunkSizeInBytes / 1024, "Kb each).");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the VolumeResampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::PagedVolume(const PagedVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented to prevent accidental copying.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume The destructor will call flushAll() to ensure that a paging volume has the chance to save it's data via the dataOverflowHandler() if desired.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>::~PagedVolume()
	{
		flushAll();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	PagedVolume<VoxelType>& PagedVolume<VoxelType>::operator=(const PagedVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented to prevent accidental copying.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param uXPos The \c x position of the voxel
	/// \param uYPos The \c y position of the voxel
	/// \param uZPos The \c z position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos & m_iChunkMask);
		const uint16_t yOffset = static_cast<uint16_t>(uYPos & m_iChunkMask);
		const uint16_t zOffset = static_cast<uint16_t>(uZPos & m_iChunkMask);

		auto pChunk = canReuseLastAccessedChunk(chunkX, chunkY, chunkZ) ? m_pLastAccessedChunk : getChunk(chunkX, chunkY, chunkZ);

		return pChunk->getVoxel(xOffset, yOffset, zOffset);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		const int32_t chunkX = uXPos >> m_uChunkSideLengthPower;
		const int32_t chunkY = uYPos >> m_uChunkSideLengthPower;
		const int32_t chunkZ = uZPos >> m_uChunkSideLengthPower;

		const uint16_t xOffset = static_cast<uint16_t>(uXPos - (chunkX << m_uChunkSideLengthPower));
		const uint16_t yOffset = static_cast<uint16_t>(uYPos - (chunkY << m_uChunkSideLengthPower));
		const uint16_t zOffset = static_cast<uint16_t>(uZPos - (chunkZ << m_uChunkSideLengthPower));

		auto pChunk = canReuseLastAccessedChunk(chunkX, chunkY, chunkZ) ? m_pLastAccessedChunk : getChunk(chunkX, chunkY, chunkZ);

		pChunk->setVoxel(xOffset, yOffset, zOffset, tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note that if the memory usage limit is not large enough to support the region this function will only load part of the region. In this case it is undefined which parts will actually be loaded. If all the voxels in the given region are already loaded, this function will not do anything. Other voxels might be unloaded to make space for the new voxels.
	/// \param regPrefetch The Region of voxels to prefetch into memory.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::prefetch(Region regPrefetch)
	{
		// Convert the start and end positions into chunk space coordinates
		Vector3DInt32 v3dStart;
		for (int i = 0; i < 3; i++)
		{
			v3dStart.setElement(i, regPrefetch.getLowerCorner().getElement(i) >> m_uChunkSideLengthPower);
		}

		Vector3DInt32 v3dEnd;
		for (int i = 0; i < 3; i++)
		{
			v3dEnd.setElement(i, regPrefetch.getUpperCorner().getElement(i) >> m_uChunkSideLengthPower);
		}

		// Ensure we don't page in more chunks than the volume can hold.
		Region region(v3dStart, v3dEnd);
		uint32_t uNoOfChunks = static_cast<uint32_t>(region.getWidthInVoxels() * region.getHeightInVoxels() * region.getDepthInVoxels());
		POLYVOX_LOG_WARNING_IF(uNoOfChunks > m_uChunkCountLimit, "Attempting to prefetch more than the maximum number of chunks (this will cause thrashing).");
		uNoOfChunks = (std::min)(uNoOfChunks, m_uChunkCountLimit);

		// Loops over the specified positions and touch the corresponding chunks.
		for (int32_t x = v3dStart.getX(); x <= v3dEnd.getX(); x++)
		{
			for (int32_t y = v3dStart.getY(); y <= v3dEnd.getY(); y++)
			{
				for (int32_t z = v3dStart.getZ(); z <= v3dEnd.getZ(); z++)
				{
					getChunk(x, y, z);
				}
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Removes all voxels from memory, and calls dataOverflowHandler() to ensure the application has a chance to store the data.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void PagedVolume<VoxelType>::flushAll()
	{
		// Clear this pointer as all chunks are about to be removed.
		m_pLastAccessedChunk = nullptr;

		// Erase all the most recently used chunks.
		for (uint32_t uIndex = 0; uIndex < uChunkArraySize; uIndex++)
		{
			m_arrayChunks[uIndex] = nullptr;
		}
	}

	template <typename VoxelType>
	bool PagedVolume<VoxelType>::canReuseLastAccessedChunk(int32_t iChunkX, int32_t iChunkY, int32_t iChunkZ) const
	{
		return ((iChunkX == m_v3dLastAccessedChunkX) &&
			(iChunkY == m_v3dLastAccessedChunkY) &&
			(iChunkZ == m_v3dLastAccessedChunkZ) &&
			(m_pLastAccessedChunk));
	}

	template <typename VoxelType>
	typename PagedVolume<VoxelType>::Chunk* PagedVolume<VoxelType>::getChunk(int32_t uChunkX, int32_t uChunkY, int32_t uChunkZ) const
	{
		Chunk* pChunk = nullptr;

		// We generate a 16-bit hash here and assume this matches the range available in the chunk
		// array. The assert here is just to make sure we take care if change this in the future.
		static_assert(uChunkArraySize == 65536, "Chunk array size has changed, check if the hash calculation needs updating.");
		// Extract the lower five bits from each position component.
		const uint32_t uChunkXLowerBits = static_cast<uint32_t>(uChunkX & 0x1F);
		const uint32_t uChunkYLowerBits = static_cast<uint32_t>(uChunkY & 0x1F);
		const uint32_t uChunkZLowerBits = static_cast<uint32_t>(uChunkZ & 0x1F);
		// Combine then to form a 15-bit hash of the position. Also shift by one to spread the values out in the whole 16-bit space.
		const uint32_t iPosisionHash = (((uChunkXLowerBits)) | ((uChunkYLowerBits) << 5) | ((uChunkZLowerBits) << 10) << 1);

		// Starting at the position indicated by the hash, and then search through the whole array looking for a chunk with the correct
		// position. In most cases we expect to find it in the first place we look. Note that this algorithm is slow in the case that
		// the chunk is not found because the whole array has to be searched, but in this case we are going to have to page the data in
		// from an external source which is likely to be slow anyway.
		uint32_t iIndex = iPosisionHash;
		do
		{
			if (m_arrayChunks[iIndex])
			{
				Vector3DInt32& entryPos = m_arrayChunks[iIndex]->m_v3dChunkSpacePosition;
				if (entryPos.getX() == uChunkX && entryPos.getY() == uChunkY && entryPos.getZ() == uChunkZ)
				{
					pChunk = m_arrayChunks[iIndex].get();
					pChunk->m_uChunkLastAccessed = ++m_uTimestamper;
					break;
				}
			}

			iIndex++;
			iIndex %= uChunkArraySize;
		} while (iIndex != iPosisionHash); // Keep searching until we get back to our start position.

		// If we still haven't found the chunk then it's time to create a new one and page it in from disk.
		if (!pChunk)
		{
			// The chunk was not found so we will create a new one.
			Vector3DInt32 v3dChunkPos(uChunkX, uChunkY, uChunkZ);
			pChunk = new PagedVolume<VoxelType>::Chunk(v3dChunkPos, m_uChunkSideLength, m_pPager);
			pChunk->m_uChunkLastAccessed = ++m_uTimestamper; // Important, as we may soon delete the oldest chunk

			// Store the chunk at the appropriate place in out chunk array. Ideally this place is
			// given by the hash, otherwise we do a linear search for the next available location
			// We always expect to find a free place because we aim to keep the array only half full.
			uint32_t iIndex = iPosisionHash;
			bool bInsertedSucessfully = false;
			do
			{
				if (m_arrayChunks[iIndex] == nullptr)
				{
					m_arrayChunks[iIndex] = std::move(std::unique_ptr< Chunk >(pChunk));
					bInsertedSucessfully = true;
					break;
				}

				iIndex++;
				iIndex %= uChunkArraySize;
			} while (iIndex != iPosisionHash); // Keep searching until we get back to our start position.

			// This should never really happen unless we are failing to keep our number of active chunks
			// significantly under the target amount. Perhaps if chunks are 'pinned' for threading purposes?
			POLYVOX_THROW_IF(!bInsertedSucessfully, std::logic_error, "No space in chunk array for new chunk.");

			// As we have added a chunk we may have exceeded our target chunk limit. Search through the array to
			// determine how many chunks we have, as well as finding the oldest timestamp. Note that this is potentially
			// wasteful and we may instead wish to track how many chunks we have and/or delete a chunk at random (or
			// just check e.g. 10 and delete the oldest of those) but we'll see if this is a bottleneck first. Paging
			// the data in is probably more expensive.
			uint32_t uChunkCount = 0;
			uint32_t uOldestChunkIndex = 0;
			uint32_t uOldestChunkTimestamp = std::numeric_limits<uint32_t>::max();
			for (uint32_t uIndex = 0; uIndex < uChunkArraySize; uIndex++)
			{
				if (m_arrayChunks[uIndex])
				{
					uChunkCount++;
					if (m_arrayChunks[uIndex]->m_uChunkLastAccessed < uOldestChunkTimestamp)
					{
						uOldestChunkTimestamp = m_arrayChunks[uIndex]->m_uChunkLastAccessed;
						uOldestChunkIndex = uIndex;
					}
				}
			}

			// Check if we have too many chunks, and delete the oldest if so.
			if (uChunkCount > m_uChunkCountLimit)
			{
				m_arrayChunks[uOldestChunkIndex] = nullptr;
			}
		}

		m_pLastAccessedChunk = pChunk;
		m_v3dLastAccessedChunkX = uChunkX;
		m_v3dLastAccessedChunkY = uChunkY;
		m_v3dLastAccessedChunkZ = uChunkZ;

		return pChunk;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Calculate the memory usage of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t PagedVolume<VoxelType>::calculateSizeInBytes(void)
	{
		uint32_t uChunkCount = 0;
		for (uint32_t uIndex = 0; uIndex < uChunkArraySize; uIndex++)
		{
			if (m_arrayChunks[uIndex])
			{
				uChunkCount++;
			}
		}

		// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
		// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
		return PagedVolume<VoxelType>::Chunk::calculateSizeInBytes(m_uChunkSideLength) * uChunkCount;
	}
}

