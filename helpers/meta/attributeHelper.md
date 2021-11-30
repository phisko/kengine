# [attributeHelper](attributeHelper.hpp)

Helper functions to manipulate [Attributes meta Components](../../components/meta/Attributes.md).

## Members

### findAttribute

```cpp
template<typename T>
const Attributes::AttributeInfo * findAttribute(std::string_view path, std::string_view separator = ".") noexcept;

const Attributes::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator = ".") noexcept;
```

Returns a pointer to the requested attribute's info, if it exists.