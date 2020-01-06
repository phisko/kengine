# [OpenGLModelComponent](OpenGLModelComponent.hpp)

`Component` that holds OpenGL-specific data regarding a ["model" Entity](ModelComponent.md).

## Specs

* Not reflectible
* Not serializable (holds OpenGL identifiers)
* Initialized by the [OpenGLSystem](../../systems/OpenGLSystem.md) and used by shaders

## Members

### Mesh type

```cpp
struct Mesh {
    GLuint vertexArrayObject = -1;
    GLuint vertexBuffer = -1;
    GLuint indexBuffer = -1;
    size_t nbIndices = 0;
    GLenum indexType = GL_UNSIGNED_INT;
};
```

Holds information for a specific mesh.

### meshes

```cpp
std::vector<Mesh> meshes;
```

List of meshes comprising the model.

### vertexRegisterFunc

```cpp
void (*vertexRegisterFunc)() = nullptr;
```

Function that will be called to initialize vertex attributes for each mesh, taken from the [ModelDataComponent](ModelDataComponent.md).