# [GodRaysComponent](GodRaysComponent.hpp)

`Component` that indicates that an `Entity` with a [LightComponent](LightComponent.md) should generate volumetric lighting, also known as "god rays".

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### intensity

```cpp
float intensity = 2.5f;
```

### scattering

```cpp
float scattering = .1f;
```

### nbSteps, defaultStepLength

```cpp
float nbSteps = 10.f;
float defaultStepLength = 25.f;
```

In the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)'s god ray shaders, volumetric lighting is achieved by performing a raycast from the camera's position for each pixel, up to the position of the object seen in that pixel. At each step of the raycast, the algorithm increments a value if the ray's position is lit, and that value indicates the total illumination for the pixel.

`nbSteps` is the number of steps in each ray cast. Higher values mean better accuracy in the volumetric lighting, but also a much larger performance cost.

`defaultStepLength` is the default length used for pixels where no object was seen. If an object was seen, the length of each step is `distanceToObject / nbSteps`.