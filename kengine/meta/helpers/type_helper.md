# [type_helper](type_helper.hpp)

Helper functions for manipulating `type entities`, i.e. entities that hold information about a specific component type.

## Members

### get_type_entity

```cpp
template<typename T>
Entity get_type_entity(entt::registry & r) noexcept;
```

Returns the `type entity` for `T`.
