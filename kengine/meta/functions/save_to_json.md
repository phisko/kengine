# [save_to_json](save_to_json.hpp)

`Meta component` that serializes the parent component into a [JSON](https://github.com/nlohmann/json) object. 

## Prototype

```cpp
nlohmann::json (entt::const_handle);
```

### Parameters

* `e`: entity whose component we want to serialize

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to parse.

A helper [register_save_to_json](../helpers/meta/impl/register_save_to_json.md) function is provided that takes as a template parameter a set of component types and implements this `meta component` for them.

Note that the implementation provided in `register_save_to_json` is only a sample, and users may freely replace it with any other implementation they desire.

