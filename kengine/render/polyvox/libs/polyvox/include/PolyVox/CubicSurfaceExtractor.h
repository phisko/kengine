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

#ifndef __PolyVox_CubicSurfaceExtractor_H__
#define __PolyVox_CubicSurfaceExtractor_H__

#include "Impl/PlatformDefinitions.h"

#include "Array.h"
#include "BaseVolume.h" //For wrap modes... should move these?
#include "DefaultIsQuadNeeded.h"
#include "Mesh.h"
#include "Vertex.h"

namespace PolyVox
{
	/// A specialised vertex format which encodes the data from the cubic extraction algorithm in a very 
	/// compact way. You will probably want to use the decodeVertex() function to turn it into a regular
	/// Vertex for rendering, but advanced users should also be able to decode it on the GPU (not tested).
	template<typename _DataType>
	struct  CubicVertex
	{
		typedef _DataType DataType;

		/// Each component of the position is stored as a single unsigned byte.
		/// The true position is found by offseting each component by 0.5f.
		Vector3DUint8 encodedPosition;

		/// A copy of the data which was stored in the voxel which generated this vertex.
		DataType data;
	};

	// Convienient shorthand for declaring a mesh of 'cubic' vertices
	// Currently disabled because it requires GCC 4.7
	//template <typename VertexDataType, typename IndexType = DefaultIndexType>
	//using CubicMesh = Mesh< CubicVertex<VertexDataType>, IndexType >;

	/// Decodes a position from a CubicVertex
	inline Vector3DFloat decodePosition(const Vector3DUint8& encodedPosition);

	/// Decodes a CubicVertex by converting it into a regular Vertex which can then be directly used for rendering.
	template<typename DataType>
	Vertex<DataType> decodeVertex(const CubicVertex<DataType>& cubicVertex);

	/// Generates a cubic-style mesh from the voxel data.
	template<typename VolumeType, typename MeshType, typename IsQuadNeeded = DefaultIsQuadNeeded<typename VolumeType::VoxelType> >
	void extractCubicMeshCustom(VolumeType* volData, Region region, MeshType* result, IsQuadNeeded isQuadNeeded = IsQuadNeeded(), bool bMergeQuads = true);

	/// Generates a cubic-style mesh from the voxel data, placing the result into a user-provided Mesh.
	template<typename VolumeType, typename IsQuadNeeded = DefaultIsQuadNeeded<typename VolumeType::VoxelType> >
	Mesh<CubicVertex<typename VolumeType::VoxelType> > extractCubicMesh(VolumeType* volData, Region region, IsQuadNeeded isQuadNeeded = IsQuadNeeded(), bool bMergeQuads = true);
	
}

#include "CubicSurfaceExtractor.inl"

#endif
