# [HighlightComponent](HighlightComponent.hpp)

`Component` indicating that an `Entity` should be highlighted.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### colorNormalized

```cpp
putils::Vector3f colorNormalized;
```

Color of the highlight. All color components should be normalized in the [0;1] range.

##### intensity

```cpp
float intensity = .5f;
```

Intensity of the highlight, normalized in the [0;1] range.