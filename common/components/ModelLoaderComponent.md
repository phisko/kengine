# [ModelComponent](ModelComponent.hpp)

Used only by model loading systems.

Provides functions to load a model into the OpenGLSystem.

### Specs

* Not serializable

### Members

##### ModelData

```cpp
struct ModelData {
	struct MeshData {
		struct DataInfo {
			size_t nbElements;
			size_t elementSize;
			const void * data;
		};

		DataInfo vertices;
		DataInfo indices;
		int indexType; // GLenum (GL_UNSIGNED_SHORT / GL_UNSIGNED_INT / ...)
	};

	std::vector<MeshData> meshes;
	putils::Vector3f offsetToCentre;
	float pitch = 0.f;
	float yaw = 0.f;
};
```

Represents a model, comprised of several meshes.

##### func

```cpp
std::function<ModelData()> func;
```

##### vertexRegisterFunc

```cpp
std::function<void(putils::gl::Program & p)> vertexRegisterFunc;
```

Function to register a vertex type with shaders, by calling their `setVertexType` or `setPolyVoxVertexType` functions.