# [load_entity](load_entity.hpp)

```cpp
void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept;
```

Deserializes `entity_json` into the existing entity.

For components to be de-serializable, the [load](../functions/load.md) `meta component` must have been registered.