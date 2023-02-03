# [register_everything](register_everything.hpp)

```cpp
template<typename... Comps>
void register_everything(entt::registry & r) noexcept;
```

Function that registers a set of types with all the engine's reflection facilities:
* [register_storage](register_storage.md)
* [register_metadata](register_metadata.md)
* [register_meta_components](register_meta_components.md)
* [register_with_script_languages](register_with_script_languages.md)