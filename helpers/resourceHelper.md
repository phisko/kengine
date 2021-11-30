# [resourceHelper](resourceHelper.hpp)

Helper functions for manipulating resources.

## Members

### loadTexture

```cpp
EntityID loadTexture(const char * file) noexcept;
```

If a [model Entity](../components/data/ModelComponent.md) already exists for `file`, returns its id. If not, loads `file` from the disk, fills a [TextureDataComponent](../components/data/TextureDataComponent.md) with it and returns the new `Entity`'s ID.

```cpp
EntityID loadTexture(void * data, size_t width, size_t height) noexcept;
```

Does the same as above, but with texture data that's already in memory.