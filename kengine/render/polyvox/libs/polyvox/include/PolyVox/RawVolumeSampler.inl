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

#define CAN_GO_NEG_X(val) (val > this->mVolume->getEnclosingRegion().getLowerX())
#define CAN_GO_POS_X(val) (val < this->mVolume->getEnclosingRegion().getUpperX())
#define CAN_GO_NEG_Y(val) (val > this->mVolume->getEnclosingRegion().getLowerY())
#define CAN_GO_POS_Y(val) (val < this->mVolume->getEnclosingRegion().getUpperY())
#define CAN_GO_NEG_Z(val) (val > this->mVolume->getEnclosingRegion().getLowerZ())
#define CAN_GO_POS_Z(val) (val < this->mVolume->getEnclosingRegion().getUpperZ())

namespace PolyVox
{
	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::Sampler(RawVolume<VoxelType>* volume)
		:BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >(volume)
		, mCurrentVoxel(0)
		, m_bIsCurrentPositionValidInX(false)
		, m_bIsCurrentPositionValidInY(false)
		, m_bIsCurrentPositionValidInZ(false)
	{
	}

	template <typename VoxelType>
	RawVolume<VoxelType>::Sampler::~Sampler()
	{
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::getVoxel(void) const
	{
		if (this->isCurrentPositionValid())
		{
			return *mCurrentVoxel;
		}
		else
		{
			return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	bool inline RawVolume<VoxelType>::Sampler::isCurrentPositionValid(void) const
	{
		return m_bIsCurrentPositionValidInX && m_bIsCurrentPositionValidInY && m_bIsCurrentPositionValidInZ;
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::setPosition(const Vector3DInt32& v3dNewPos)
	{
		setPosition(v3dNewPos.getX(), v3dNewPos.getY(), v3dNewPos.getZ());
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::setPosition(int32_t xPos, int32_t yPos, int32_t zPos)
	{
		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::setPosition(xPos, yPos, zPos);

		m_bIsCurrentPositionValidInX = this->mVolume->getEnclosingRegion().containsPointInX(xPos);
		m_bIsCurrentPositionValidInY = this->mVolume->getEnclosingRegion().containsPointInY(yPos);
		m_bIsCurrentPositionValidInZ = this->mVolume->getEnclosingRegion().containsPointInZ(zPos);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid())
		{
			const Vector3DInt32& v3dLowerCorner = this->mVolume->m_regValidRegion.getLowerCorner();
			int32_t iLocalXPos = xPos - v3dLowerCorner.getX();
			int32_t iLocalYPos = yPos - v3dLowerCorner.getY();
			int32_t iLocalZPos = zPos - v3dLowerCorner.getZ();

			const int32_t uVoxelIndex = iLocalXPos +
				iLocalYPos * this->mVolume->getWidth() +
				iLocalZPos * this->mVolume->getWidth() * this->mVolume->getHeight();

			mCurrentVoxel = this->mVolume->m_pData + uVoxelIndex;
		}
		else
		{
			mCurrentVoxel = 0;
		}
	}

	template <typename VoxelType>
	bool RawVolume<VoxelType>::Sampler::setVoxel(VoxelType tValue)
	{
		//return m_bIsCurrentPositionValid ? *mCurrentVoxel : this->mVolume->getBorderValue();
		if (this->m_bIsCurrentPositionValidInX && this->m_bIsCurrentPositionValidInY && this->m_bIsCurrentPositionValidInZ)
		{
			*mCurrentVoxel = tValue;
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveX(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveX();

		m_bIsCurrentPositionValidInX = this->mVolume->getEnclosingRegion().containsPointInX(this->mXPosInVolume);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid() && bIsOldPositionValid)
		{
			++mCurrentVoxel;
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveY(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveY();

		m_bIsCurrentPositionValidInY = this->mVolume->getEnclosingRegion().containsPointInY(this->mYPosInVolume);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid() && bIsOldPositionValid)
		{
			mCurrentVoxel += this->mVolume->getWidth();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::movePositiveZ(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::movePositiveZ();

		m_bIsCurrentPositionValidInZ = this->mVolume->getEnclosingRegion().containsPointInZ(this->mZPosInVolume);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid() && bIsOldPositionValid)
		{
			mCurrentVoxel += this->mVolume->getWidth() * this->mVolume->getHeight();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeX(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeX();

		m_bIsCurrentPositionValidInX = this->mVolume->getEnclosingRegion().containsPointInX(this->mXPosInVolume);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid() && bIsOldPositionValid)
		{
			--mCurrentVoxel;
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeY(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeY();

		m_bIsCurrentPositionValidInY = this->mVolume->getEnclosingRegion().containsPointInY(this->mYPosInVolume);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid() && bIsOldPositionValid)
		{
			mCurrentVoxel -= this->mVolume->getWidth();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	void RawVolume<VoxelType>::Sampler::moveNegativeZ(void)
	{
		// We'll need this in a moment...
		bool bIsOldPositionValid = this->isCurrentPositionValid();

		// Base version updates position and validity flags.
		BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> >::moveNegativeZ();

		m_bIsCurrentPositionValidInZ = this->mVolume->getEnclosingRegion().containsPointInZ(this->mZPosInVolume);

		// Then we update the voxel pointer
		if (this->isCurrentPositionValid() && bIsOldPositionValid)
		{
			mCurrentVoxel -= this->mVolume->getWidth() * this->mVolume->getHeight();
		}
		else
		{
			setPosition(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
		}
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1ny1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume))
		{
			return *(mCurrentVoxel - 1);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx0py1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1nx1py1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume - 1, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_Y(this->mYPosInVolume))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1ny1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py0pz(void) const
	{
		if ((this->isCurrentPositionValid()))
		{
			return *mCurrentVoxel;
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px0py1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_Y(this->mYPosInVolume))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel0px1py1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume, this->mYPosInVolume + 1, this->mZPosInVolume + 1);
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1ny1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume - 1, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + 1 - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume))
		{
			return *(mCurrentVoxel + 1);
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px0py1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume, this->mZPosInVolume + 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1nz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_NEG_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() - this->mVolume->getWidth() * this->mVolume->getHeight());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume - 1);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py0pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth());
		}
		return this->mVolume->getVoxel(this->mXPosInVolume + 1, this->mYPosInVolume + 1, this->mZPosInVolume);
	}

	template <typename VoxelType>
	VoxelType RawVolume<VoxelType>::Sampler::peekVoxel1px1py1pz(void) const
	{
		if ((this->isCurrentPositionValid()) && CAN_GO_POS_X(this->mXPosInVolume) && CAN_GO_POS_Y(this->mYPosInVolume) && CAN_GO_POS_Z(this->mZPosInVolume))
		{
			return *(mCurrentVoxel + 1 + this->mVolume->getWidth() + this->mVolume->getWidth() * this->mVolume->getHeight());
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
