# [MagicaVoxelSystem](MagicaVoxelSystem.hpp)

System that loads 3D models for `Entities` with a [GraphicsComponent](../../components/data/GraphicsComponent.md) by parsing the [MagicaVoxel format](https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt). 3D models are generated through the `PolyVox` library, with the vertex format found in the [PolyVoxComponent](../../components/data/PolyVoxComponent.md).

### Shader

The `MagicaVoxelSystem` does not create a shader to render the models it loads. Instead, the shader from the [PolyVoxSystem](PolyVoxSystem.md) should be used.