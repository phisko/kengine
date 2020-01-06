# [ModelDataComponent](ModelDataComponent.hpp)

Used only by model loading systems.

Provides data to loaded for a model into the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md).

## Specs

* Not reflectible
* Not serializable
* Initialized by model loading systems (such as the [AssImpSystem](../../systems/assimp/AssImpSystem.md)) and processed by the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md).

## Members

### Mesh

```cpp
struct Mesh {
	struct DataInfo {
		size_t nbElements;
		size_t elementSize;
		const void * data;
	};

	DataInfo vertices;
	DataInfo indices;
	int indexType; // GLenum (GL_UNSIGNED_SHORT / GL_UNSIGNED_INT / ...)
};
```

Holds vertex information about a mesh.

### meshes

```cpp
std::vector<MeshData> meshes;
```

Represents a model, made up of several meshes.

### free

```cpp
putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE> free;
```

Called to release the model data once it has been loaded into the OpenGL context.

The maximum size for the `free` functor defaults to 64 and can be adjusted by defining the `KENGINE_MODEL_LOADER_FUNCTION_SIZE` macro.

### vertexRegisterFunc

```cpp
void (*vertexRegisterFunc)();
```

Function to set the OpenGL vertex array buffer attributes, which can be done by calling `putils::gl::setVertexType` or `putils::gl::setPolyVoxVertexType` functions.