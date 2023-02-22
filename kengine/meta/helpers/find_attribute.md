# [attribute_helper](attribute_helper.hpp)

Helper functions to inspect the [attributes](../data/attributes.md) `meta component`.

## Members

### find_attribute

```cpp
const attributes::attribute_info * find_attribute(entt::handle type_entity, std::string_view path, std::string_view separator = ".") noexcept;
```

Returns a pointer to the requested attribute's info, if it exists.