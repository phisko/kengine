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

#include "Impl/Utility.h"

namespace PolyVox
{
	template <typename VoxelType>
	template <typename DerivedVolumeType>
	BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::Sampler(DerivedVolumeType* volume)
		:mVolume(volume)
		, mXPosInVolume(0)
		, mYPosInVolume(0)
		, mZPosInVolume(0)
	{
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::~Sampler()
	{
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	Vector3DInt32 BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::getPosition(void) const
	{
		return Vector3DInt32(mXPosInVolume, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::getVoxel(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		mXPosInVolume = xPos;
		mYPosInVolume = yPos;
		mZPosInVolume = zPos;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	bool BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::setVoxel(VoxelType tValue)
	{
		return mVolume->setVoxel(mXPosInVolume, mYPosInVolume, mZPosInVolume, tValue);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveX(void)
	{
		mXPosInVolume++;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveY(void)
	{
		mYPosInVolume++;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::movePositiveZ(void)
	{
		mZPosInVolume++;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeX(void)
	{
		mXPosInVolume--;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeY(void)
	{
		mYPosInVolume--;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	void BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::moveNegativeZ(void)
	{
		mZPosInVolume--;
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume - 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume - 1, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1ny1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume - 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx0py1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume + 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume + 1, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1nx1py1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume - 1, mYPosInVolume + 1, mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume - 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume - 1, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1ny1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume - 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px0py1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume + 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume + 1, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel0px1py1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume, mYPosInVolume + 1, mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume - 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume - 1, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1ny1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume - 1, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px0py1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume, mZPosInVolume + 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py1nz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume + 1, mZPosInVolume - 1);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py0pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume + 1, mZPosInVolume);
	}

	template <typename VoxelType>
	template <typename DerivedVolumeType>
	VoxelType BaseVolume<VoxelType>::Sampler<DerivedVolumeType>::peekVoxel1px1py1pz(void) const
	{
		return mVolume->getVoxel(mXPosInVolume + 1, mYPosInVolume + 1, mZPosInVolume + 1);
	}
}
