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
	/// This constructor creates a volume with a fixed size which is specified as a parameter.
	/// \param regValid Specifies the minimum and maximum valid voxel positions.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::RawVolume(const Region& regValid)
		:BaseVolume<VoxelType>()
		, m_regValidRegion(regValid)
		, m_tBorderValue()
	{
			this->setBorderValue(VoxelType());

			//Create a volume of the right size.
			initialise(regValid);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::RawVolume(const RawVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume copy constructor not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Destroys the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>::~RawVolume()
	{
		delete[] m_pData;
		m_pData = 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should never be called. Copying volumes by value would be expensive, and we want to prevent users from doing
	/// it by accident (such as when passing them as paramenters to functions). That said, there are times when you really do want to
	/// make a copy of a volume and in this case you should look at the Volumeresampler.
	///
	/// \sa VolumeResampler
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	RawVolume<VoxelType>& RawVolume<VoxelType>::operator=(const RawVolume<VoxelType>& /*rhs*/)
	{
		POLYVOX_THROW(not_implemented, "Volume assignment operator not implemented for performance reasons.");
	}

	////////////////////////////////////////////////////////////////////////////////
	/// The border value is returned whenever an attempt is made to read a voxel which
	/// is outside the extents of the volume.
	/// \return The value used for voxels outside of the volume
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getBorderValue(void) const
	{
		return m_tBorderValue;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return A Region representing the extent of the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	const Region& RawVolume<VoxelType>::getEnclosingRegion(void) const
	{
		return m_regValidRegion;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The width of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the width is 64.
	/// \sa getHeight(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t RawVolume<VoxelType>::getWidth(void) const
	{
		return m_regValidRegion.getUpperX() - m_regValidRegion.getLowerX() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The height of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the height is 64.
	/// \sa getWidth(), getDepth()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t RawVolume<VoxelType>::getHeight(void) const
	{
		return m_regValidRegion.getUpperY() - m_regValidRegion.getLowerY() + 1;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \return The depth of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the depth is 64.
	/// \sa getWidth(), getHeight()
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	int32_t RawVolume<VoxelType>::getDepth(void) const
	{
		return m_regValidRegion.getUpperZ() - m_regValidRegion.getLowerZ() + 1;
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
	VoxelType RawVolume<VoxelType>::getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if (this->m_regValidRegion.containsPoint(uXPos, uYPos, uZPos))
		{
			const Region& regValidRegion = this->m_regValidRegion;
			int32_t iLocalXPos = uXPos - regValidRegion.getLowerX();
			int32_t iLocalYPos = uYPos - regValidRegion.getLowerY();
			int32_t iLocalZPos = uZPos - regValidRegion.getLowerZ();

			return m_pData
				[
					iLocalXPos +
					iLocalYPos * this->getWidth() +
					iLocalZPos * this->getWidth() * this->getHeight()
				];
		}
		else
		{
			return m_tBorderValue;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This version of the function is provided so that the wrap mode does not need
	/// to be specified as a template parameter, as it may be confusing to some users.
	/// \param v3dPos The 3D position of the voxel
	/// \return The voxel value
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::getVoxel(const Vector3DInt32& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param tBorder The value to use for voxels outside the volume.
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::setBorderValue(const VoxelType& tBorder)
	{
		m_tBorderValue = tBorder;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param uXPos the \c x position of the voxel
	/// \param uYPos the \c y position of the voxel
	/// \param uZPos the \c z position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		if (this->m_regValidRegion.containsPoint(Vector3DInt32(uXPos, uYPos, uZPos)) == false)
		{
			POLYVOX_THROW(std::out_of_range, "Position is outside valid region");
		}

		const Vector3DInt32& v3dLowerCorner = this->m_regValidRegion.getLowerCorner();
		int32_t iLocalXPos = uXPos - v3dLowerCorner.getX();
		int32_t iLocalYPos = uYPos - v3dLowerCorner.getY();
		int32_t iLocalZPos = uZPos - v3dLowerCorner.getZ();

		m_pData
			[
				iLocalXPos +
				iLocalYPos * this->getWidth() +
				iLocalZPos * this->getWidth() * this->getHeight()
			] = tValue;
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \param v3dPos the 3D position of the voxel
	/// \param tValue the value to which the voxel will be set
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue)
	{
		setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// This function should probably be made internal...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	void RawVolume<VoxelType>::initialise(const Region& regValidRegion)
	{
		this->m_regValidRegion = regValidRegion;

		if (this->getWidth() <= 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume width must be greater than zero.");
		}
		if (this->getHeight() <= 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume height must be greater than zero.");
		}
		if (this->getDepth() <= 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume depth must be greater than zero.");
		}

		//Create the data
		m_pData = new VoxelType[this->getWidth() * this->getHeight()* this->getDepth()];

		// Clear to zeros
		std::fill(m_pData, m_pData + this->getWidth() * this->getHeight()* this->getDepth(), VoxelType());
	}

	////////////////////////////////////////////////////////////////////////////////
	/// Note: This function needs reviewing for accuracy...
	////////////////////////////////////////////////////////////////////////////////
	template <typename VoxelType>
	uint32_t RawVolume<VoxelType>::calculateSizeInBytes(void)
	{
		return this->getWidth() * this->getHeight() * this->getDepth() * sizeof(VoxelType);
	}
}

