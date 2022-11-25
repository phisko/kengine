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

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	/// This is protected because you should never create a BaseVolume directly, you should instead use one of the derived classes.
	///
	/// \sa RawVolume, PagedVolume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>::BaseVolume()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the VolumeResampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>::BaseVolume(const BaseVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented to prevent accidental copying.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>::~BaseVolume()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the VolumeResampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	BaseVolume<VoxelType>& BaseVolume<VoxelType>::operator=(const BaseVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented to prevent accidental copying.");
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
	VoxelType BaseVolume<VoxelType>::getVoxel(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/) const
	{
		POLYVOX_ASSERT(false, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType BaseVolume<VoxelType>::getVoxel(const Vector3DInt32& /*v3dPos*/) const
	{
		POLYVOX_ASSERT(false, "You should never call the base class version of this function.");
		return VoxelType();
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void BaseVolume<VoxelType>::setVoxel(int32_t /*uXPos*/, int32_t /*uYPos*/, int32_t /*uZPos*/, VoxelType /*tValue*/)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void BaseVolume<VoxelType>::setVoxel(const Vector3DInt32& /*v3dPos*/, VoxelType /*tValue*/)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// 
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t BaseVolume<VoxelType>::calculateSizeInBytes(void)
	{
		POLYVOX_THROW(not_implemented, "You should never call the base class version of this function.");
		return 0;
	}
}

