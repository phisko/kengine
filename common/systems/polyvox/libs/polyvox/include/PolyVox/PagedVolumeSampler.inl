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

#include <array>

#define CAN_GO_NEG_X(val) (val > 0)
#define CAN_GO_POS_X(val)  (val < this->m_uChunkSideLengthMinusOne)
#define CAN_GO_NEG_Y(val) (val > 0)
#define CAN_GO_POS_Y(val)  (val < this->m_uChunkSideLengthMinusOne)
#define CAN_GO_NEG_Z(val) (val > 0)
#define CAN_GO_POS_Z(val)  (val < this->m_uChunkSideLengthMinusOne)

#define NEG_X_DELTA (-(deltaX[this->m_uXPosInChunk-1]))
#define POS_X_DELTA (deltaX[this->m_uXPosInChunk])
#define NEG_Y_DELTA (-(deltaY[this->m_uYPosInChunk-1]))
#define POS_Y_DELTA (deltaY[this->m_uYPosInChunk])
#define NEG_Z_DELTA (-(deltaZ[this->m_uZPosInChunk-1]))
#define POS_Z_DELTA (deltaZ[this->m_uZPosInChunk])

namespace PolyVox
{
	// These precomputed offset are used to determine how much we move our pointer by to move a single voxel in the x, y, or z direction given an x, y, or z starting position inside a chunk.
	// More information in this discussion: https://bitbucket.org/volumesoffun/polyvox/issue/61/experiment-with-morton-ordering-of-voxel
	static const std::array<int32_t, 256> deltaX = { 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 1797559, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1 };
	static const std::array<int32_t, 256> deltaY = { 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 3595118, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2 };
	static const std::array<int32_t, 256> deltaZ = { 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 7190236, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4 };

	template <typename VoxelType>
	PagedVolume<VoxelType>::Sampler::Sampler(PagedVolume<VoxelType>* volume)
		:BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >(volume), m_uChunkSideLengthMinusOne(volume->m_uChunkSideLength - 1)
	{
	}

	template <typename VoxelType>
	PagedVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		return *mCurrentVoxel;
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::setPosition(xPos, yPos, zPos);

		// Then we update the voxel pointer
		const int32_t uXChunk = this->mXPosInVolume >> this->mVolume->m_uChunkSideLengthPower;
		const int32_t uYChunk = this->mYPosInVolume >> this->mVolume->m_uChunkSideLengthPower;
		const int32_t uZChunk = this->mZPosInVolume >> this->mVolume->m_uChunkSideLengthPower;

		m_uXPosInChunk = static_cast<uint16_t>(this->mXPosInVolume - (uXChunk << this->mVolume->m_uChunkSideLengthPower));
		m_uYPosInChunk = static_cast<uint16_t>(this->mYPosInVolume - (uYChunk << this->mVolume->m_uChunkSideLengthPower));
		m_uZPosInChunk = static_cast<uint16_t>(this->mZPosInVolume - (uZChunk << this->mVolume->m_uChunkSideLengthPower));

		uint32_t uVoxelIndexInChunk = morton256_x[m_uXPosInChunk] | morton256_y[m_uYPosInChunk] | morton256_z[m_uZPosInChunk];

		auto pCurrentChunk = this->mVolume->canReuseLastAccessedChunk(uXChunk, uYChunk, uZChunk) ?
			this->mVolume->m_pLastAccessedChunk : this->mVolume->getChunk(uXChunk, uYChunk, uZChunk);

		mCurrentVoxel = pCurrentChunk->m_tData + uVoxelIndexInChunk;
	}

	template <typename VoxelType>
	bool PagedVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		//Need to think what effect this has on any existing iterators.
		POLYVOX_THROW(not_implemented, "This function cannot be used on PagedVolume samplers.");
		return false;
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::movePositiveX(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::movePositiveX();

		// Then we update the voxel pointer
		if (CAN_GO_POS_X(this->m_uXPosInChunk))
		{
			//No need to compute new chunk.
			mCurrentVoxel += POS_X_DELTA;
			this->m_uXPosInChunk++;
		}
		else
		{
			//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::movePositiveY();

		// Then we update the voxel pointer
		if (CAN_GO_POS_Y(this->m_uYPosInChunk))
		{
			//No need to compute new chunk.
			mCurrentVoxel += POS_Y_DELTA;
			this->m_uYPosInChunk++;
		}
		else
		{
			//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::movePositiveZ();

		// Then we update the voxel pointer
		if (CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			//No need to compute new chunk.
			mCurrentVoxel += POS_Z_DELTA;
			this->m_uZPosInChunk++;
		}
		else
		{
			//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::moveNegativeX();

		// Then we update the voxel pointer
		if (CAN_GO_NEG_X(this->m_uXPosInChunk))
		{
			//No need to compute new chunk.
			mCurrentVoxel += NEG_X_DELTA;
			this->m_uXPosInChunk--;
		}
		else
		{
			//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::moveNegativeY();

		// Then we update the voxel pointer
		if (CAN_GO_NEG_Y(this->m_uYPosInChunk))
		{
			//No need to compute new chunk.
			mCurrentVoxel += NEG_Y_DELTA;
			this->m_uYPosInChunk--;
		}
		else
		{
			//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void PagedVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< PagedVolume<VoxelType> >::moveNegativeZ();

		// Then we update the voxel pointer
		if (CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			//No need to compute new chunk.
			mCurrentVoxel += NEG_Z_DELTA;
			this->m_uZPosInChunk--;
		}
		else
		{
			//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_NEG_Y(this->m_uYPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + NEG_Y_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_NEG_Y(this->m_uYPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + NEG_Y_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_NEG_Y(this->m_uYPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + NEG_Y_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_POS_Y(this->m_uYPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + POS_Y_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_POS_Y(this->m_uYPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + POS_Y_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if (CAN_GO_NEG_X(this->m_uXPosInChunk) && CAN_GO_POS_Y(this->m_uYPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_X_DELTA + POS_Y_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if (CAN_GO_NEG_Y(this->m_uYPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_Y_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if (CAN_GO_NEG_Y(this->m_uYPosInChunk))
		{
			return *(mCurrentVoxel + NEG_Y_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if (CAN_GO_NEG_Y(this->m_uYPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_Y_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if (CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		return *mCurrentVoxel;
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if (CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if (CAN_GO_POS_Y(this->m_uYPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_Y_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if (CAN_GO_POS_Y(this->m_uYPosInChunk))
		{
			return *(mCurrentVoxel + POS_Y_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if (CAN_GO_POS_Y(this->m_uYPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_Y_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_NEG_Y(this->m_uYPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + NEG_Y_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_NEG_Y(this->m_uYPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + NEG_Y_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_NEG_Y(this->m_uYPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + NEG_Y_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_POS_Y(this->m_uYPosInChunk) && CAN_GO_NEG_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + POS_Y_DELTA + NEG_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_POS_Y(this->m_uYPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + POS_Y_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType PagedVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if (CAN_GO_POS_X(this->m_uXPosInChunk) && CAN_GO_POS_Y(this->m_uYPosInChunk) && CAN_GO_POS_Z(this->m_uZPosInChunk))
		{
			return *(mCurrentVoxel + POS_X_DELTA + POS_Y_DELTA + POS_Z_DELTA);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
	}
}

#undef CAN_GO_NEG_X
#undef CAN_GO_POS_X
#undef CAN_GO_NEG_Y
#undef CAN_GO_POS_Y
#undef CAN_GO_NEG_Z
#undef CAN_GO_POS_Z

#undef NEG_X_DELTA
#undef POS_X_DELTA
#undef NEG_Y_DELTA
#undef POS_Y_DELTA
#undef NEG_Z_DELTA
#undef POS_Z_DELTA