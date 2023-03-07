# [asset](asset.hpp)

Component that represents an asset file.

Typically attached to [model entities](../../model).

## Members

### file

```cpp
putils::string<KENGINE_RENDER_ASSET_STRING_MAX_LENGTH> file;
```

The file this component describes.

The maximum length of a filename (stored as a putils::string) defaults to 64, and can be adjusted by defining the KENGINE_RENDER_ASSET_STRING_MAX_LENGTH macro.