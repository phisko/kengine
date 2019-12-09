# [LightComponent](LightComponent.hpp)

`Components` that let entities emit light.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable

### Common part

##### color

```cpp
putils::Vector3f color;
```
Normalized color.

##### diffuseStrength, specularStrength

```cpp
float diffuseStrength;
float specularStrength;
```

### DirLightComponent

Used to represent a directional light, such as the sun.

##### direction

```cpp
putils::Vector3f direction;
```

##### ambientStrength

```cpp
float ambientStrength;
```

### PointLightComponent

Used to represent a point light, such as a light bulb.

##### constant, linear, quadratic

```cpp
float constant = 1.f;
float linear = .09f;
float quadratic = .032f;
```

Reference values provided by [Ogre3D](http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation).

### SpotLightComponent

Used to represent a spot light, such as a flash light. Inherits from `PointLightComponent`.

##### direction

```cpp
putils::Vector3f direction;
```

##### cutOff, outerCutoff

```cpp
float cutOff = std::cos(12.5f);
float outerCutOff = std::cos(15.f);
```