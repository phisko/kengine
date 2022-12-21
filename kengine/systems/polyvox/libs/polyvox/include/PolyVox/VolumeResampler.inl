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

#include "Impl/Interpolation.h"

#include <cmath>

namespace PolyVox
{
	/**
	 * \param pVolSrc
	 * \param regSrc
	 * \param[out] pVolDst
	 * \param regDst
	 */
	template< typename SrcVolumeType, typename DstVolumeType>
	VolumeResampler<SrcVolumeType, DstVolumeType>::VolumeResampler(SrcVolumeType* pVolSrc, const Region &regSrc, DstVolumeType* pVolDst, const Region& regDst)
		:m_pVolSrc(pVolSrc)
		, m_regSrc(regSrc)
		, m_pVolDst(pVolDst)
		, m_regDst(regDst)
	{
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::execute()
	{
		int32_t uSrcWidth = m_regSrc.getUpperX() - m_regSrc.getLowerX() + 1;
		int32_t uSrcHeight = m_regSrc.getUpperY() - m_regSrc.getLowerY() + 1;
		int32_t uSrcDepth = m_regSrc.getUpperZ() - m_regSrc.getLowerZ() + 1;

		int32_t uDstWidth = m_regDst.getUpperX() - m_regDst.getLowerX() + 1;
		int32_t uDstHeight = m_regDst.getUpperY() - m_regDst.getLowerY() + 1;
		int32_t uDstDepth = m_regDst.getUpperZ() - m_regDst.getLowerZ() + 1;

		if ((uSrcWidth == uDstWidth) && (uSrcHeight == uDstHeight) && (uSrcDepth == uDstDepth))
		{
			resampleSameSize();
		}
		else
		{
			resampleArbitrary();
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleSameSize()
	{
		for (int32_t sz = m_regSrc.getLowerZ(), dz = m_regDst.getLowerZ(); dz <= m_regDst.getUpperZ(); sz++, dz++)
		{
			for (int32_t sy = m_regSrc.getLowerY(), dy = m_regDst.getLowerY(); dy <= m_regDst.getUpperY(); sy++, dy++)
			{
				for (int32_t sx = m_regSrc.getLowerX(), dx = m_regDst.getLowerX(); dx <= m_regDst.getUpperX(); sx++, dx++)
				{
					const typename SrcVolumeType::VoxelType& tSrcVoxel = m_pVolSrc->getVoxel(sx, sy, sz);
					const typename DstVolumeType::VoxelType& tDstVoxel = static_cast<typename DstVolumeType::VoxelType>(tSrcVoxel);
					m_pVolDst->setVoxel(dx, dy, dz, tDstVoxel);
				}
			}
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType>
	void VolumeResampler<SrcVolumeType, DstVolumeType>::resampleArbitrary()
	{
		float srcWidth = m_regSrc.getWidthInCells();
		float srcHeight = m_regSrc.getHeightInCells();
		float srcDepth = m_regSrc.getDepthInCells();

		float dstWidth = m_regDst.getWidthInCells();
		float dstHeight = m_regDst.getHeightInCells();
		float dstDepth = m_regDst.getDepthInCells();

		float fScaleX = srcWidth / dstWidth;
		float fScaleY = srcHeight / dstHeight;
		float fScaleZ = srcDepth / dstDepth;

		typename SrcVolumeType::Sampler sampler(m_pVolSrc);

		for (int32_t dz = m_regDst.getLowerZ(); dz <= m_regDst.getUpperZ(); dz++)
		{
			for (int32_t dy = m_regDst.getLowerY(); dy <= m_regDst.getUpperY(); dy++)
			{
				for (int32_t dx = m_regDst.getLowerX(); dx <= m_regDst.getUpperX(); dx++)
				{
					float sx = (dx - m_regDst.getLowerX()) * fScaleX;
					float sy = (dy - m_regDst.getLowerY()) * fScaleY;
					float sz = (dz - m_regDst.getLowerZ()) * fScaleZ;

					sx += m_regSrc.getLowerX();
					sy += m_regSrc.getLowerY();
					sz += m_regSrc.getLowerZ();

					sampler.setPosition(sx, sy, sz);
					const typename SrcVolumeType::VoxelType& voxel000 = sampler.peekVoxel0px0py0pz();
					const typename SrcVolumeType::VoxelType& voxel001 = sampler.peekVoxel0px0py1pz();
					const typename SrcVolumeType::VoxelType& voxel010 = sampler.peekVoxel0px1py0pz();
					const typename SrcVolumeType::VoxelType& voxel011 = sampler.peekVoxel0px1py1pz();
					const typename SrcVolumeType::VoxelType& voxel100 = sampler.peekVoxel1px0py0pz();
					const typename SrcVolumeType::VoxelType& voxel101 = sampler.peekVoxel1px0py1pz();
					const typename SrcVolumeType::VoxelType& voxel110 = sampler.peekVoxel1px1py0pz();
					const typename SrcVolumeType::VoxelType& voxel111 = sampler.peekVoxel1px1py1pz();

					//FIXME - should accept all float parameters, but GCC complains?
					double dummy;
					sx = modf(sx, &dummy);
					sy = modf(sy, &dummy);
					sz = modf(sz, &dummy);

					typename SrcVolumeType::VoxelType tInterpolatedValue = trilerp<float>(voxel000, voxel100, voxel010, voxel110, voxel001, voxel101, voxel011, voxel111, sx, sy, sz);

					typename DstVolumeType::VoxelType result = static_cast<typename DstVolumeType::VoxelType>(tInterpolatedValue);
					m_pVolDst->setVoxel(dx, dy, dz, result);
				}
			}
		}
	}
}
