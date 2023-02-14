# [json_helper](json_helper.hpp)

Helper functions for serializing and de-serializing entities to JSON.

## Usage

These functions are implemented in terms of `meta components`.

For components to be serializable, the [save_to_json](../meta/save_to_json.md) `meta component` must first have been registered for them. To be de-serializable, the [load_from_json](../meta/load_from_json.md) `meta component` must have been registered.

## Members

### load_entity

```cpp
void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept;
```

Deserializes `entity_json` into the existing entity.

### save_entity

```cpp
nlohmann::json save_entity(entt::const_handle e) noexcept;
```

Returns a JSON object generated by serializing the entity.