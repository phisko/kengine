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

#include "PolyVox/RawVolume.h" // Currently used by exectureSAT() method - should be replaced by PagedVolume or a template parameter?

namespace PolyVox
{
	/**
	 * \param pVolSrc
	 * \param regSrc
	 * \param[out] pVolDst
	 * \param regDst
	 * \param uKernelSize
	 */
	template< typename SrcVolumeType, typename DstVolumeType, typename AccumulationType>
	LowPassFilter<SrcVolumeType, DstVolumeType, AccumulationType>::LowPassFilter(SrcVolumeType* pVolSrc, Region regSrc, DstVolumeType* pVolDst, Region regDst, uint32_t uKernelSize)
		:m_pVolSrc(pVolSrc)
		, m_regSrc(regSrc)
		, m_pVolDst(pVolDst)
		, m_regDst(regDst)
		, m_uKernelSize(uKernelSize)
	{
		//Kernel size must be at least three
		if (m_uKernelSize < 3)
		{
			POLYVOX_THROW(std::invalid_argument, "Kernel size must be at least three");
		}

		//Kernel size must be odd
		if (m_uKernelSize % 2 == 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Kernel size must be odd");
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType, typename AccumulationType>
	void LowPassFilter<SrcVolumeType, DstVolumeType, AccumulationType>::execute()
	{
		int32_t iSrcMinX = m_regSrc.getLowerX();
		int32_t iSrcMinY = m_regSrc.getLowerY();
		int32_t iSrcMinZ = m_regSrc.getLowerZ();

		int32_t iSrcMaxX = m_regSrc.getUpperX();
		int32_t iSrcMaxY = m_regSrc.getUpperY();
		int32_t iSrcMaxZ = m_regSrc.getUpperZ();

		int32_t iDstMinX = m_regDst.getLowerX();
		int32_t iDstMinY = m_regDst.getLowerY();
		int32_t iDstMinZ = m_regDst.getLowerZ();

		//int32_t iDstMaxX = m_regDst.getUpperX();
		//int32_t iDstMaxY = m_regDst.getUpperY();
		//int32_t iDstMaxZ = m_regDst.getUpperZ();

		typename SrcVolumeType::Sampler srcSampler(m_pVolSrc);

		for (int32_t iSrcZ = iSrcMinZ, iDstZ = iDstMinZ; iSrcZ <= iSrcMaxZ; iSrcZ++, iDstZ++)
		{
			for (int32_t iSrcY = iSrcMinY, iDstY = iDstMinY; iSrcY <= iSrcMaxY; iSrcY++, iDstY++)
			{
				for (int32_t iSrcX = iSrcMinX, iDstX = iDstMinX; iSrcX <= iSrcMaxX; iSrcX++, iDstX++)
				{
					AccumulationType tSrcVoxel(0);
					srcSampler.setPosition(iSrcX, iSrcY, iSrcZ);

					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1ny1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1ny0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1ny1pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx0py1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx0py0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx0py1pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1py1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1py0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1nx1py1pz());

					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1ny1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1ny0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1ny1pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px0py1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px0py0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px0py1pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1py1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1py0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel0px1py1pz());

					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1ny1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1ny0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1ny1pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px0py1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px0py0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px0py1pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1py1nz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1py0pz());
					tSrcVoxel += static_cast<AccumulationType>(srcSampler.peekVoxel1px1py1pz());

					tSrcVoxel /= 27;

					//tSrcVoxel.setDensity(uDensity);
					m_pVolDst->setVoxel(iSrcX, iSrcY, iSrcZ, static_cast<typename DstVolumeType::VoxelType>(tSrcVoxel));
				}
			}
		}
	}

	template< typename SrcVolumeType, typename DstVolumeType, typename AccumulationType>
	void LowPassFilter<SrcVolumeType, DstVolumeType, AccumulationType>::executeSAT()
	{
		const uint32_t border = (m_uKernelSize - 1) / 2;

		Vector3DInt32 satLowerCorner = m_regSrc.getLowerCorner() - Vector3DInt32(border, border, border);
		Vector3DInt32 satUpperCorner = m_regSrc.getUpperCorner() + Vector3DInt32(border, border, border);

		//Use floats for the SAT volume to ensure it works with negative
		//densities and with both integral and floating point input volumes.
		RawVolume<AccumulationType> satVolume(Region(satLowerCorner, satUpperCorner));

		//Clear to zeros (necessary?)
		//FIXME - use Volume::fill() method. Implemented in base class as below
		//but with optimised implementations in subclasses?
		for (int32_t z = satLowerCorner.getZ(); z <= satUpperCorner.getZ(); z++)
		{
			for (int32_t y = satLowerCorner.getY(); y <= satUpperCorner.getY(); y++)
			{
				for (int32_t x = satLowerCorner.getX(); x <= satUpperCorner.getX(); x++)
				{
					satVolume.setVoxel(x, y, z, 0);
				}
			}
		}

		typename RawVolume<AccumulationType>::Sampler satVolumeIter(&satVolume);

		IteratorController<typename RawVolume<AccumulationType>::Sampler> satIterCont;
		satIterCont.m_regValid = Region(satLowerCorner, satUpperCorner);
		satIterCont.m_Iter = &satVolumeIter;
		satIterCont.reset();

		typename SrcVolumeType::Sampler srcVolumeIter(m_pVolSrc);

		IteratorController<typename SrcVolumeType::Sampler> srcIterCont;
		srcIterCont.m_regValid = Region(satLowerCorner, satUpperCorner);
		srcIterCont.m_Iter = &srcVolumeIter;
		srcIterCont.reset();

		do
		{
			AccumulationType previousSum = static_cast<AccumulationType>(satVolumeIter.peekVoxel1nx0py0pz());
			AccumulationType currentVal = static_cast<AccumulationType>(srcVolumeIter.getVoxel());

			satVolumeIter.setVoxel(previousSum + currentVal);

			srcIterCont.moveForward();

		} while (satIterCont.moveForward());

		//Build SAT in three passes
		/*for(int32_t z = satLowerCorner.getZ(); z <= satUpperCorner.getZ(); z++)
		{
		for(int32_t y = satLowerCorner.getY(); y <= satUpperCorner.getY(); y++)
		{
		for(int32_t x = satLowerCorner.getX(); x <= satUpperCorner.getX(); x++)
		{
		AccumulationType previousSum = static_cast<AccumulationType>(satVolume.getVoxel(x-1,y,z));
		AccumulationType currentVal = static_cast<AccumulationType>(m_pVolSrc->getVoxel(x,y,z));

		satVolume.setVoxel(x,y,z,previousSum + currentVal);
		}
		}
		}*/

		for (int32_t z = satLowerCorner.getZ(); z <= satUpperCorner.getZ(); z++)
		{
			for (int32_t y = satLowerCorner.getY(); y <= satUpperCorner.getY(); y++)
			{
				for (int32_t x = satLowerCorner.getX(); x <= satUpperCorner.getX(); x++)
				{
					AccumulationType previousSum = static_cast<AccumulationType>(satVolume.getVoxel(x, y - 1, z));
					AccumulationType currentSum = static_cast<AccumulationType>(satVolume.getVoxel(x, y, z));

					satVolume.setVoxel(x, y, z, previousSum + currentSum);
				}
			}
		}

		for (int32_t z = satLowerCorner.getZ(); z <= satUpperCorner.getZ(); z++)
		{
			for (int32_t y = satLowerCorner.getY(); y <= satUpperCorner.getY(); y++)
			{
				for (int32_t x = satLowerCorner.getX(); x <= satUpperCorner.getX(); x++)
				{
					AccumulationType previousSum = static_cast<AccumulationType>(satVolume.getVoxel(x, y, z - 1));
					AccumulationType currentSum = static_cast<AccumulationType>(satVolume.getVoxel(x, y, z));

					satVolume.setVoxel(x, y, z, previousSum + currentSum);
				}
			}
		}

		//Now compute the average
		const Vector3DInt32& v3dDstLowerCorner = m_regDst.getLowerCorner();
		const Vector3DInt32& v3dDstUpperCorner = m_regDst.getUpperCorner();

		const Vector3DInt32& v3dSrcLowerCorner = m_regSrc.getLowerCorner();

		for (int32_t iDstZ = v3dDstLowerCorner.getZ(), iSrcZ = v3dSrcLowerCorner.getZ(); iDstZ <= v3dDstUpperCorner.getZ(); iDstZ++, iSrcZ++)
		{
			for (int32_t iDstY = v3dDstLowerCorner.getY(), iSrcY = v3dSrcLowerCorner.getY(); iDstY <= v3dDstUpperCorner.getY(); iDstY++, iSrcY++)
			{
				for (int32_t iDstX = v3dDstLowerCorner.getX(), iSrcX = v3dSrcLowerCorner.getX(); iDstX <= v3dDstUpperCorner.getX(); iDstX++, iSrcX++)
				{
					int32_t satLowerX = iSrcX - border - 1;
					int32_t satLowerY = iSrcY - border - 1;
					int32_t satLowerZ = iSrcZ - border - 1;

					int32_t satUpperX = iSrcX + border;
					int32_t satUpperY = iSrcY + border;
					int32_t satUpperZ = iSrcZ + border;

					AccumulationType a = satVolume.getVoxel(satLowerX, satLowerY, satLowerZ);
					AccumulationType b = satVolume.getVoxel(satUpperX, satLowerY, satLowerZ);
					AccumulationType c = satVolume.getVoxel(satLowerX, satUpperY, satLowerZ);
					AccumulationType d = satVolume.getVoxel(satUpperX, satUpperY, satLowerZ);
					AccumulationType e = satVolume.getVoxel(satLowerX, satLowerY, satUpperZ);
					AccumulationType f = satVolume.getVoxel(satUpperX, satLowerY, satUpperZ);
					AccumulationType g = satVolume.getVoxel(satLowerX, satUpperY, satUpperZ);
					AccumulationType h = satVolume.getVoxel(satUpperX, satUpperY, satUpperZ);

					AccumulationType sum = h + c - d - g - f - a + b + e;
					uint32_t sideLength = border * 2 + 1;
					AccumulationType average = sum / (sideLength*sideLength*sideLength);

					m_pVolDst->setVoxel(iDstX, iDstY, iDstZ, static_cast<typename DstVolumeType::VoxelType>(average));
				}
			}
		}
	}
}
