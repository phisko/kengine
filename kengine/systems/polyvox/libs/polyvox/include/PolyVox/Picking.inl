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

#include "Raycast.h"

namespace PolyVox
{
	namespace
	{
		/**
		 * This is just an implementation class for the pickVoxel function
		 *
		 * It makes note of the sort of empty voxel you're looking for in the constructor.
		 *
		 * Each time the operator() is called:
		 *  * if it's hit a voxel it sets up the result and returns false
		 *  * otherwise it preps the result for the next iteration and returns true
		 */
		template <typename VolumeType>
		class RaycastPickingFunctor
		{
		public:
			RaycastPickingFunctor(const typename VolumeType::VoxelType& emptyVoxelExample)
				:m_emptyVoxelExample(emptyVoxelExample)
				, m_result()
			{
			}

			bool operator()(const typename VolumeType::Sampler& sampler)
			{
				if (sampler.getVoxel() != m_emptyVoxelExample) //If we've hit something
				{
					m_result.didHit = true;
					m_result.hitVoxel = sampler.getPosition();
					return false;
				}

				m_result.previousVoxel = sampler.getPosition();

				return true;
			}
			const typename VolumeType::VoxelType& m_emptyVoxelExample;
			PickResult m_result;
		};
	}

	/**
	 * \param volData The volume to pass the ray though
	 * \param v3dStart The start position in the volume
	 * \param v3dDirectionAndLength The direction and length of the ray
	 * \param emptyVoxelExample The value used to represent empty voxels in your volume
	 *
	 * \return A PickResult containing the hit information
	 */
	template<typename VolumeType>
	PickResult pickVoxel(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, const typename VolumeType::VoxelType& emptyVoxelExample)
	{
		RaycastPickingFunctor<VolumeType> functor(emptyVoxelExample);

		raycastWithDirection(volData, v3dStart, v3dDirectionAndLength, functor);

		return functor.m_result;
	}
}
