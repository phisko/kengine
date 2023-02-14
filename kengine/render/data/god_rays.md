# [god_rays](god_rays.hpp)

Component that draws volumetric lighting (also known as "god rays") for the [light entity](light.md).

## Members

### scattering

```cpp
float scattering = .1f;
```

### nb_steps, default_step_length

```cpp
int nb_steps = 10;
float default_step_length = 25.f;
```

### intensity

```cpp
float intensity = 2.5f;
```

In [kreogl](https://github.com/phisko/kreogl)'s god ray shaders, volumetric lighting is achieved by performing a raycast from the camera's position for each pixel, up to the position of the object seen in that pixel. At each step of the raycast, the algorithm increments a value if the ray's position is lit, and that value indicates the total illumination for the pixel.

`nb_steps` is the number of steps in each ray cast. Higher values mean better accuracy in the volumetric lighting, but also a much larger performance cost.

`default_step_length` is the default length used for pixels where no object was seen. If an object was seen, the length of each step is `distance_to_object / nb_steps`.