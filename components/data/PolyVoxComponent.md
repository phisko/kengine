# [PolyVoxComponent](PolyVoxComponent.hpp)

Stores a volume of voxels that can get transformed into a 3D model.

The length, width and height of the voxel volume default to 16 and can be adjusted by defining the `KENGINE_POLYVOX_CHUNK_SIDE` macro.

## Specs

* Not reflectible
* Not serializable
* Processed by the [PolyVoxSystem](../../systems/polyvox/PolyVoxSystem.md)

## Members

### VertexData

```cpp
struct VertexData {
    float color[3];
};
```

Each voxel simply provides its color.

### volume

```cpp
PolyVox::RawVolume<VertexData> volume;
```

Voxel volume that can be manipulated by users.

### changed

```cpp
bool changed;
```

Indicates whether the volume has changed. Setting this to `true` indicates that the 3D model should be regenerated.