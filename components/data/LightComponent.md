# [LightComponent](LightComponent.hpp)

`Components` that let entities emit light.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems

## Common part

### color

```cpp
putils::Vector3f color;
```
Normalized color.

### diffuseStrength, specularStrength

```cpp
float diffuseStrength;
float specularStrength;
```

### castShadows

```cpp
bool castShadows = true;
```

### shadowPCFSamples

```cpp
int shadowPCFSamples = 1;
```

Number of samples to take into account for [Percentage Closer Filtering](https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch11.html). Increasing this will reduce shadow aliasing at the cost of GPU processing time.

### shadowMapSize

```cpp
int shadowMapSize = 1024;
```

Size of the shadow map texture. Increasing this will increase shadow accuracy at the cost of GPU memory.

### shadowMapBias

```cpp
float shadowMapMaxBias = .1f;
float shadowMapMinBias = .01f;
```

Bias applied when reading the shadow map. Increasing this will reduce "shadow acne", but also increase the risk of shadows not being applied properly.

## DirLightComponent

Used to represent a directional light, such as the sun.

### direction

```cpp
putils::Vector3f direction;
```

### ambientStrength

```cpp
float ambientStrength;
```

### lightSphereDistance

```cpp
float lightSphereDistance = 500.f;
```

The distance at which a sphere should be drawn to symbolize this.

### cascadeEnds

```cpp
float cascadeEnds[KENGINE_CSM_COUNT] = { 50.f };
```

Distances for each "cascade" of the [Cascaded Shadow Map](https://docs.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps).

## PointLightComponent

Used to represent a point light, such as a light bulb.

### attenuationConstant, attenuationLinear, attenuationQuadratic

```cpp
float attenuationConstant = 1.f;
float attenuationLinear = .09f;
float attenuationQuadratic = .032f;
```

Reference values provided by [Ogre3D](http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation).

## SpotLightComponent

Used to represent a spot light, such as a flash light. Inherits from `PointLightComponent`.

### direction

```cpp
putils::Vector3f direction;
```

### cutOff, outerCutoff

```cpp
float cutOff = std::cos(12.5f);
float outerCutOff = std::cos(15.f);
```