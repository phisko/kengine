# [ModelDataComponent](ModelDataComponent.hpp)

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
};
```

Represents a model, comprised of several meshes.

##### func

```cpp
putils::function<ModelData(), KENGINE_MODEL_LOADER_FUNCTION_SIZE> func;
```

Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_MODEL_LOADER_FUNCTION_SIZE` macro.

##### vertexRegisterFunc

```cpp
void (*vertexRegisterFunc)();
```

Function to set the OpenGL vertex array buffer attributes, which can be done by calling `putils::gl::setVertexType` or `putils::gl::setPolyVoxVertexType` functions.
