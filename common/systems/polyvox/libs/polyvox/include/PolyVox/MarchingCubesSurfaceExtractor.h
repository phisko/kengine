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

#ifndef __PolyVox_SurfaceExtractor_H__
#define __PolyVox_SurfaceExtractor_H__

#include "Impl/MarchingCubesTables.h"
#include "Impl/PlatformDefinitions.h"

#include "Array.h"
#include "DefaultMarchingCubesController.h"
#include "Mesh.h"
#include "Vertex.h"

namespace PolyVox
{
	/// A specialised vertex format which encodes the data from the Marching Cubes algorithm in a very 
	/// compact way. You will probably want to use the decodeVertex() function to turn it into a regular
	/// Vertex for rendering, but advanced users can also decode it on the GPU (see PolyVox examples).
	template<typename _DataType>
	struct  MarchingCubesVertex
	{		
		typedef _DataType DataType;

		/// Each component of the position is stored using 8.8 fixed-point encoding.
		Vector3DUint16 encodedPosition;

		/// The normal is encoded as a 16-bit unsigned integer using the 'oct16'
		/// encoding described here: http://jcgt.org/published/0003/02/01/
		uint16_t encodedNormal;

		/// The interpolated voxel data from the neighbouring voxels which generated this
		/// vertex (every vertex is placed between two voxels by the MArching Cubes algorithm)
		DataType data;
	};

	// Convienient shorthand for declaring a mesh of marching cubes vertices
	// Currently disabled because it requires GCC 4.7
	//template <typename VertexDataType, typename IndexType = DefaultIndexType>
	//using MarchingCubesMesh = Mesh< MarchingCubesVertex<VertexDataType>, IndexType >;

	/// Decodes a MarchingCubesVertex by converting it into a regular Vertex which can then be directly used for rendering.
	template<typename DataType>
	Vertex<DataType> decodeVertex(const MarchingCubesVertex<DataType>& marchingCubesVertex);

	/// Generates a mesh from the voxel data using the Marching Cubes algorithm.
	template< typename VolumeType, typename ControllerType = DefaultMarchingCubesController<typename VolumeType::VoxelType> >
	Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > extractMarchingCubesMesh(VolumeType* volData, Region region, ControllerType controller = ControllerType());

	/// Generates a mesh from the voxel data using the Marching Cubes algorithm, placing the result into a user-provided Mesh.
	template< typename VolumeType, typename MeshType, typename ControllerType = DefaultMarchingCubesController<typename VolumeType::VoxelType> >
	void extractMarchingCubesMeshCustom(VolumeType* volData, Region region, MeshType* result, ControllerType controller = ControllerType());
}

#include "MarchingCubesSurfaceExtractor.inl"

#endif
