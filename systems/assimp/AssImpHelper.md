# [AssImpHelper](AssImpHelper.hpp)

Helper functions and types for working with AssImp.

## AssImpObjectComponent

Tag `Component` indicating that an object should be drawn by the [AssImpShader](AssImpShader.md).

## AssImpTexturesModelComponent

`Component` attached to [model Entities](../../components/data/ModelComponent.md) that lists the textures used by its model.

## drawModel

```cpp
void drawModel(const InstanceComponent & instance, const TransformComponent & transform, const SkeletonComponent & skeleton, bool useTextures, const Uniforms & uniforms) noexcept;
```

Draws an `Entity` after having uploaded its skinning information.