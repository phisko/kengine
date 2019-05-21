**********************
Tutorial 1 - Basic use
**********************

Introduction
============
This tutorial covers the basic use of the PolyVox API. After reading this tutorial you should have a good idea how to create a PolyVox volume and fill it with data, extract a triangle mesh representing the surface, and render the result. This tutorial assumes you are already familiar with the basic concepts behind PolyVox (see the :doc:`principles of polyvox <principles>` document if not), and are reasonably confident with 3D graphics and C++. It also assumes you have already got PolyVox installed on your system, if this is not the case then please consult :doc:`installation guide <install>`.

The code samples and text in this tutorial correspond directly to the BasicExample which comes with PolyVox. This example uses the Qt toolkit for window and input handling, and for providing an OpenGL context to render into. In this tutorial we will omit code which performs these tasks and will instead focus on on the PolyVox code. You can consult the `Qt documentation <http://doc.qt.nokia.com/latest/>`_ if you want more information about these other aspects of the system.

Creating a volume
=================
To get started, we need to include the following headers:

.. sourcecode:: c++

	#include "PolyVox/CubicSurfaceExtractor.h"
	#include "PolyVox/MarchingCubesSurfaceExtractor.h"
	#include "PolyVox/Mesh.h"
	#include "PolyVox/RawVolume.h"

The most fundamental construct when working with PolyVox is that of the volume. This is represented by the :polyvox:`RawVolume` class which stores a 3D grid of voxels. Our basic example application creates a volume with the following line of code:

.. sourcecode:: c++

	RawVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(63, 63, 63)));

As can be seen, the RawVolume class is templated upon the voxel type. This means it is straightforward to create a volume of integers, floats, or a custom voxel type (see the :polyvox:`RawVolume documentation <PolyVox::RawVolume>` for more details). In this particular case we have created a volume in which each voxel is of type `uint8_t` which is an unsigned 8-bit integer.

Next, we set some of the voxels in the volume to be 'solid' in order to create a large sphere in the centre of the volume. We do this with the following function call:

.. sourcecode:: c++

	createSphereInVolume(volData, 30);

Note that this function is part of the BasicExample (rather than being part of the PolyVox library) and is implemented as follows:
	
.. sourcecode:: c++
	
	void createSphereInVolume(RawVolume<uint8_t>& volData, float fRadius)
	{
		//This vector hold the position of the center of the volume
		Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

		//This three-level for loop iterates over every voxel in the volume
		for (int z = 0; z < volData.getDepth(); z++)
		{
			for (int y = 0; y < volData.getHeight(); y++)
			{
				for (int x = 0; x < volData.getWidth(); x++)
				{
					//Store our current position as a vector...
					Vector3DFloat v3dCurrentPos(x, y, z);
					//And compute how far the current position is from the center of the volume
					float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

					uint8_t uVoxelValue = 0;

					//If the current voxel is less than 'radius' units from the center then we make it solid.
					if (fDistToCenter <= fRadius)
					{
						//Our new voxel value
						uVoxelValue = 255;
					}

					//Wrte the voxel value into the volume	
					volData.setVoxel(x, y, z, uVoxelValue);
				}
			}
		}
	}
	
This function takes as input the :polyvox:`RawVolume` in which we want to create the sphere, and also a radius specifying how large we want the sphere to be. In our case we have specified a radius of 30 voxels, which will fit nicely inside our :polyvox:`RawVolume` of dimensions 64x64x64.

Because this is a simple example function it always places the sphere at the centre of the volume. It computes this centre by halving the dimensions of the volume as given by the functions :polyvox:`SimpleVolume::getWidth`, :polyvox:`SimpleVolume::getHeight` and :polyvox:`SimpleVolume::getDepth`. The resulting position is stored using a :polyvox:`Vector3DFloat`. This is simply a typedef from our templatised :polyvox:`Vector` class, meaning that other sizes and storage types are available if you need them. 

Next, the function uses a three-level 'for' loop to iterate over each voxel in the volume. For each voxel it computes the distance from the voxel to the centre of the volume. If this distance is less than or equal to the specified radius then the voxel forms part of the sphere and is made solid.

Extracting the surface
======================
Now that we have built our volume we need to convert it into a triangle mesh for rendering. This process can be performed by the :polyvox:`extractCubicMesh` function:

.. sourcecode:: c++

	auto mesh = extractCubicMesh(&volData, volData.getEnclosingRegion());

The :polyvox:`extractCubicMesh` function takes a pointer to the volume data, and also it needs to be told which :polyvox:`Region` of the volume the extraction should be performed on (in more advanced applications this is useful for extracting only those parts of the volume which have been modified since the last extraction). For our purpose the :polyvox:`RawVolume` class provides a convenient :polyvox:`RawVolume::getEnclosingRegion` function which returns a :polyvox:`Region` representing the whole volume.

The resulting mesh has a complex templatized type and so we assign it to a variable declared with 'auto', This way the compiler will determine the correct type for us. PolyVox also makes use of some compression techniques to store the vertex data in a compact way. Therefore the vertices of the mesh need to be decompressed ('decoded') before they can be used. For now the easiest approach is to use the provided ``decode()`` function, though advanced users can actually do this decoding on the GPU (see 'DecodeOnGPUExample'):

.. sourcecode:: c++

	auto decodedMesh = decodeMesh(mesh);
	
Our ``decodedMesh`` variable contains an index and vertex buffer representing the desired triangle mesh.

Note: If you like you can try swapping the :polyvox:`extractCubicMesh` for :polyvox:`extractMarchingCubesMesh`. We have already included the relevant header, and in the BasicExample you just need to change which line in commented out. The :polyvox:`MarchingCubesSurfaceExtractor` makes use of a smooth density field and will consider a voxel to be solid if it is above a threshold of half the voxel's maximum value (so in this case that's half of 255, which is 127).

Rendering the surface
=====================
Rendering the surface with OpenGL is handled by our ``PolyVoxExample`` which is an ``OpenGLWidget`` subclass. Again, this is not part of PolyVox, it is simply an example based on Qt and OpenGL which demonstrates how rendering can be performed. Within this class there are mainly two functions which are of interest - the PolyVoxExample::addMesh() function which constructs OpenGL buffers from our :polyvox:`Mesh` and the PolyVoxExample::renderOneFrame() function which is called each frame to perform the rendering.

The PolyVoxExample::addMesh() function is implemented as follows:

.. sourcecode:: c++

	template <typename MeshType>
	void addMesh(const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f)
	{
		// This struct holds the OpenGL properties (buffer handles, etc) which will be used
		// to render our mesh. We copy the data from the PolyVox mesh into this structure.
		OpenGLMeshData meshData;

		// Create the VAO for the mesh
		glGenVertexArrays(1, &(meshData.vertexArrayObject));
		glBindVertexArray(meshData.vertexArrayObject);

		// The GL_ARRAY_BUFFER will contain the list of vertex positions
		glGenBuffers(1, &(meshData.vertexBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, surfaceMesh.getNoOfVertices() * sizeof(typename MeshType::VertexType), surfaceMesh.getRawVertexData(), GL_STATIC_DRAW);

		// and GL_ELEMENT_ARRAY_BUFFER will contain the indices
		glGenBuffers(1, &(meshData.indexBuffer));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceMesh.getNoOfIndices() * sizeof(typename MeshType::IndexType), surfaceMesh.getRawIndexData(), GL_STATIC_DRAW);

		// Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
		glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, position))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)

		// Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
		// does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
		// shader. This is mostly just to simplify this example code - in a real application you will know whether your
		// chosen surface extractor generates normals and can skip uploading them if not.
		glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, normal)));

		// Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code 
		// we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
		glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
		GLint size = (std::min)(sizeof(typename MeshType::VertexType::DataType), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
		glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, data)));

		// We're done uploading and can now unbind.
		glBindVertexArray(0);

		// A few additional properties can be copied across for use during rendering.
		meshData.noOfIndices = surfaceMesh.getNoOfIndices();
		meshData.translation = QVector3D(translation.getX(), translation.getY(), translation.getZ());
		meshData.scale = scale;

		// Set 16 or 32-bit index buffer size.
		meshData.indexType = sizeof(typename MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		// Now add the mesh to the list of meshes to render.
		addMeshData(meshData);
	}
	
With the OpenGL index and vertex buffers set up, we can now look at the code which is called each frame to render them:

.. sourcecode:: c++

	void renderOneFrame() override
	{
		// Our example framework only uses a single shader for the scene (for all meshes).
		mShader->bind();

		// These two matrices are constant for all meshes.
		mShader->setUniformValue("viewMatrix", viewMatrix());
		mShader->setUniformValue("projectionMatrix", projectionMatrix());

		// Iterate over each mesh which the user added to our list, and render it.
		for (OpenGLMeshData meshData : mMeshData)
		{
			//Set up the model matrrix based on provided translation and scale.
			QMatrix4x4 modelMatrix;
			modelMatrix.translate(meshData.translation);
			modelMatrix.scale(meshData.scale);
			mShader->setUniformValue("modelMatrix", modelMatrix);

			// Bind the vertex array for the current mesh
			glBindVertexArray(meshData.vertexArrayObject);
			// Draw the mesh
			glDrawElements(GL_TRIANGLES, meshData.noOfIndices, meshData.indexType, 0);
			// Unbind the vertex array.
			glBindVertexArray(0);
		}

		// We're done with the shader for this frame.
		mShader->release();
	}
	
Again, the explanation of this code is best left to the OpenGL documentation.