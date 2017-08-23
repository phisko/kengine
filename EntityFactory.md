# [EntityFactory](EntityFactory.hpp)

Used to create `GameObjects` by providing their type as a string instead of a template parameter. This can be useful when the type of entity to create is known only at runtime.

A sample [ExtensibleFactory](ExtensibleFactory.md) is provided, which can be used for most projects.

### Members

##### make

```cpp
virtual std::unique_ptr<GameObject> make(std::string_view type, std::string_view name) = 0;
```
