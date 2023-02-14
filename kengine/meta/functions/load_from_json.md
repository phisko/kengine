# [load_from_json](load_from_json.hpp)

`Meta component` that parses the parent component from a [JSON](https://github.com/nlohmann/json) object and attaches it to a given entity.

## Prototype

```cpp
void (const nlohmann::json & json, entt::handle e);
```

### Parameters

* `json`: JSON object for `e`, NOT specifically for the parent component
* `e`: entity which the new component should be attached to

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to parse.

A helper [register_load_from_json](../helpers/meta/impl/register_load_from_json.md) function is provided that takes as a template parameter a set of component types and implements this `meta component` for it.

Note that the implementation provided in `register_load_from_json` is only a sample, and users may freely replace it with any other implementation they desire.

