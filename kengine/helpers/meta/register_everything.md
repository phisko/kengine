# [register_everything](register_everything.hpp)

```cpp
template<bool IsComponent, typename... Comps>
void register_everything(entt::registry & r) noexcept;
```

Function that registers a set of types with all the engine's reflection facilities:
* [register_with_script_languages](register_with_script_languages.md)
* [register_meta_components](register_meta_components.md) if `IsComponent` is `true`