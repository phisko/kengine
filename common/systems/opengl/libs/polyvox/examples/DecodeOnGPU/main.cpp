/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 David Williams and Matthew Williams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include "PolyVoxExample.h"

#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/RawVolume.h"

#include <QApplication>

//Use the PolyVox namespace
using namespace PolyVox;

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

class DecodeOnGPUExample : public PolyVoxExample
{
public:
	DecodeOnGPUExample(QWidget *parent)
		:PolyVoxExample(parent)
	{
	}

protected:
	void initializeExample() override
	{
		QSharedPointer<QGLShaderProgram> shader(new QGLShaderProgram);

		if (!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/decode.vert"))
		{
			std::cerr << shader->log().toStdString() << std::endl;
			exit(EXIT_FAILURE);
		}

		if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/decode.frag"))
		{
			std::cerr << shader->log().toStdString() << std::endl;
			exit(EXIT_FAILURE);
		}

		setShader(shader);

		//Create an empty volume and then place a sphere in it
		RawVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(63, 63, 63)));
		createSphereInVolume(volData, 30);

		// Extract the surface for the specified region of the volume. Uncomment the line for the kind of surface extraction you want to see.
		auto mesh = extractMarchingCubesMesh(&volData, volData.getEnclosingRegion());

		//Pass the surface to the OpenGL window
		OpenGLMeshData meshData = buildOpenGLMeshData(mesh);
		addMeshData(meshData);

		setCameraTransform(QVector3D(100.0f, 100.0f, 100.0f), -(PI / 4.0f), PI + (PI / 4.0f));
	}

private:
	OpenGLMeshData buildOpenGLMeshData(const PolyVox::Mesh< PolyVox::MarchingCubesVertex< uint8_t > >& surfaceMesh, const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f)
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
		glBufferData(GL_ARRAY_BUFFER, surfaceMesh.getNoOfVertices() * sizeof(MarchingCubesVertex< uint8_t >), surfaceMesh.getRawVertexData(), GL_STATIC_DRAW);

		// and GL_ELEMENT_ARRAY_BUFFER will contain the indices
		glGenBuffers(1, &(meshData.indexBuffer));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceMesh.getNoOfIndices() * sizeof(uint32_t), surfaceMesh.getRawIndexData(), GL_STATIC_DRAW);

		// Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
		glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
		glVertexAttribIPointer(0, 3, GL_UNSIGNED_SHORT, sizeof(MarchingCubesVertex< uint8_t >), (GLvoid*)(offsetof(MarchingCubesVertex< uint8_t >, encodedPosition))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)

		// Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
		// does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
		// shader. This is mostly just to simplify this example code - in a real application you will know whether your
		// chosen surface extractor generates normals and can skip uploading them if not.
		glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_SHORT, sizeof(MarchingCubesVertex< uint8_t >), (GLvoid*)(offsetof(MarchingCubesVertex< uint8_t >, encodedNormal)));

		// Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code 
		// we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
		glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
		GLint size = (std::min)(sizeof(uint8_t), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
		glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(MarchingCubesVertex< uint8_t >), (GLvoid*)(offsetof(MarchingCubesVertex< uint8_t >, data)));

		// We're done uploading and can now unbind.
		glBindVertexArray(0);

		// A few additional properties can be copied across for use during rendering.
		meshData.noOfIndices = surfaceMesh.getNoOfIndices();
		meshData.translation = QVector3D(translation.getX(), translation.getY(), translation.getZ());
		meshData.scale = scale;

		// Set 16 or 32-bit index buffer size.
		meshData.indexType = sizeof(PolyVox::Mesh< PolyVox::MarchingCubesVertex< uint8_t > >::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		return meshData;
	}
};

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	DecodeOnGPUExample openGLWidget(0);
	openGLWidget.show();

	//Run the message pump.
	return app.exec();
}