# [AssImpSystem](AssImpSystem.hpp)

System that loads 3D models for `Entities` with a [GraphicsComponent](../../components/GraphicsComponent.md) using the `assimp library`.

### Shader

The `AssImpSystem` automatically creates a [TexturedShader](TexturedShader.hpp) which is able to render the models it loads.

### Vertex format

```cpp
struct Vertex {
	float position[3];
	float normal[3];
	float texCoords[2];
};
```