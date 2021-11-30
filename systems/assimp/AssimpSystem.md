# [AssImpSystem](AssImpSystem.hpp)

System that loads 3D models for `Entities` with a [GraphicsComponent](../../components/GraphicsComponent.md) using the `assimp` library. The system also takes care of animating the `Entities` after they have been created.

## Shader

The `AssImpSystem` automatically creates an [AssImpShader](AssImpShader.hpp) which is able to render the models it loads.

## Vertex format

```cpp
struct Vertex {
	float position[3];
	float normal[3];
	float texCoords[2];
	float boneWeights[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX];
	unsigned int boneIDs[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX];
};
```