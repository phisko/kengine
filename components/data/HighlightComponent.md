# [HighlightComponent](HighlightComponent.hpp)

`Component` indicating that an `Entity` should be highlighted.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems

## Members

### color

```cpp
putils::NormalizedColor color;
```

Stored as a [putils::NormalizedColor](https://github.com/phisko/putils/blob/master/Color.md).

### intensity

```cpp
float intensity = 1.f;
```