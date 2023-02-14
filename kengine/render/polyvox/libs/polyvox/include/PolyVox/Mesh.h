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

#ifndef __PolyVox_Mesh_H__
#define __PolyVox_Mesh_H__

#include "Impl/PlatformDefinitions.h"

#include "Region.h"
#include "Vertex.h" //Should probably do away with this one in the future...

#include <algorithm>
#include <cstdlib>
#include <list>
#include <memory>
#include <set>
#include <vector>

namespace PolyVox
{
	/// A simple and general-purpose mesh class to represent the data returned by the surface extraction functions.
	/// It supports different vertex types (which will vary depending on the surface extractor used and the contents
	/// of the volume) and both 16-bit and 32 bit indices.
	typedef uint32_t DefaultIndexType;
	template <typename _VertexType, typename _IndexType = DefaultIndexType>
	class Mesh
	{
	public:

		typedef _VertexType VertexType;
		typedef _IndexType IndexType;

		Mesh();
		~Mesh();

		IndexType getNoOfVertices(void) const;
		const VertexType& getVertex(IndexType index) const;
		const VertexType* getRawVertexData(void) const;

		size_t getNoOfIndices(void) const;
		IndexType getIndex(uint32_t index) const;
		const IndexType* getRawIndexData(void) const;

		const Vector3DInt32& getOffset(void) const;
		void setOffset(const Vector3DInt32& offset);

		IndexType addVertex(const VertexType& vertex);
		void addTriangle(IndexType index0, IndexType index1, IndexType index2);

		void clear(void);
		bool isEmpty(void) const;
		void removeUnusedVertices(void);

	private:
		std::vector<IndexType> m_vecIndices;
		std::vector<VertexType> m_vecVertices;
		Vector3DInt32 m_offset;
	};

	/// Meshes returned by the surface extractors often have vertices with efficient compressed
	/// formats which are hard to interpret directly (see CubicVertex and MarchingCubesVertex).
	/// This function creates a new uncompressed mesh containing the much simpler Vertex objects.
	template <typename MeshType>
	Mesh< Vertex< typename MeshType::VertexType::DataType >, typename MeshType::IndexType > decodeMesh(const MeshType& encodedMesh)
	{
		Mesh< Vertex< typename MeshType::VertexType::DataType >, typename MeshType::IndexType > decodedMesh;

		for (typename MeshType::IndexType ct = 0; ct < encodedMesh.getNoOfVertices(); ct++)
		{
			decodedMesh.addVertex(decodeVertex(encodedMesh.getVertex(ct)));
		}

		POLYVOX_ASSERT(encodedMesh.getNoOfIndices() % 3 == 0, "The number of indices must always be a multiple of three.");
		for (uint32_t ct = 0; ct < encodedMesh.getNoOfIndices(); ct += 3)
		{
			decodedMesh.addTriangle(encodedMesh.getIndex(ct), encodedMesh.getIndex(ct + 1), encodedMesh.getIndex(ct + 2));
		}

		decodedMesh.setOffset(encodedMesh.getOffset());

		return decodedMesh;
	}
}

#include "Mesh.inl"

#endif /* __Mesh_H__ */
