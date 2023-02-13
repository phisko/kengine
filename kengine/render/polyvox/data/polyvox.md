# [polyvox](polyvox.hpp)

Stores a volume of voxels that can get transformed into a 3D model.

The length, width and height of the voxel volume default to 16 and can be adjusted by defining the `KENGINE_POLYVOX_CHUNK_SIDE` macro.

## Members

### vertex_data

```cpp
struct vertex_data {
    float color[3];
};
```

Each voxel simply provides its color.

### volume

```cpp
PolyVox::RawVolume<vertex_data> volume;
```

Voxel volume that can be manipulated by users.

### changed

```cpp
bool changed;
```

Indicates whether the volume has changed. Setting this to `true` indicates that the 3D model should be regenerated.