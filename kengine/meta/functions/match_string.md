# [match_string](match_string.hpp)

`Meta component` returning whether the parent component contained in a given entity matches a token string.

## Prototype

```cpp
bool (entt::const_handle e, const char * str);
```

### Return value

* `true` if `str` is found in the parent component's name
* `true` if `str` is found in any of the fields in `e`'s parent component
* `false` otherwise

### Parameters

* `e`: entity in which `str` should be searched
* `str`: token string to search for in `e`'s parent component

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to parse.

A helper [register_match_string](../helpers/meta/impl/register_match_string.md) function is provided that takes as a template parameter a set of component types and implements the `match_string` `meta component` for it.

Note that the implementation provided in `register_match_string` is only a sample, and users may freely replace it with any other implementation they desire.