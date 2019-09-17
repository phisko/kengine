# [ModelInfoComponent](ModelInfoComponent.hpp)

Used only by low-level 3D systems and custom shaders.

`Component` holding OpenGL mesh information about a 3D model. See the diagram in [ModelComponent](ModelComponent.md).

### Specs

* Not serializable

### Members

##### Mesh

```cpp
struct Mesh {
    GLuint vertexArrayObject;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    size_t nbIndices;
    GLenum indexType;
};
```

Represents one of the parts of the model.

##### meshes

```cpp
std::vector<Mesh> meshes;
```

##### translation, pitch, yaw

```cpp
glm::vec3 translation;
float pitch;
float yaw;
```

Indicates transformations to apply in addition to that of the `Entity`'s `TransformComponent`'s (for meshes that are not in the expected direction, for instance).

##### vertexRegisterFunc

```cpp
void (*vertexRegisterFunc)();
```

Function to set the OpenGL vertex array buffer attributes, which can be done by calling `putils::gl::setVertexType` or `putils::gl::setPolyVoxVertexType` functions.