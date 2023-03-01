# [light](light.hpp)

Components that let entities emit light.

## Common part

### color

```cpp
putils::normalized_color color;
```
Normalized color.

### diffuse_strength, specular_strength

```cpp
float diffuse_strength;
float specular_strength;
```

### cast_shadows

```cpp
bool cast_shadows = true;
```

### shadow_pcf_samples

```cpp
int shadow_pcf_samples = 1;
```

Number of samples to take into account for [Percentage Closer Filtering](https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch11.html). Increasing this will reduce shadow aliasing at the cost of GPU processing time.

### shadow_map_size

```cpp
int shadow_map_size = 1024;
```

Size of the shadow map texture. Increasing this will increase shadow accuracy at the cost of GPU memory.

### shadowMapBias

```cpp
float shadow_map_max_bias = .1f;
float shadow_map_min_bias = .01f;
```

Bias applied when reading the shadow map. Increasing this will reduce "shadow acne", but also increase the risk of shadows not being applied properly.

## dir_light

Used to represent a directional light, such as the sun.

### direction

```cpp
putils::vec3f direction;
```

### ambient_strength

```cpp
float ambient_strength = .1f;
```

### light_sphere_distance

```cpp
float light_sphere_distance = 500.f;
```

The distance at which a sphere should be drawn to symbolize this.

### cascade_ends

```cpp
putils::vector<float, KENGINE_MAX_CSM_COUNT> cascade_ends = { 50.f };
```

Distances for each "cascade" of the [cascaded shadow map](https://docs.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps).

### shadow_cast_max_distance

```cpp
float shadow_caster_max_distance = 100.f;
```

The distance by which to extend the calculated frustum when drawing to the shadow map. This helps take into account large off-screen objects which should cast a shadow.

## point_light

Used to represent a point light, such as a light bulb.

### attenuation_constant, attenuation_linear, attenuation_quadratic

```cpp
float attenuation_constant = 1.f;
float attenuation_linear = .09f;
float attenuation_quadratic = .032f;
```

Reference values provided by [Ogre3D](http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation).

## spot_light

Used to represent a spot light, such as a flash light. Inherits from `point_light`.

### direction

```cpp
putils::vec3f direction;
```

### cut_off, outer_cut_off

```cpp
float cut_off = 1.f;
float outer_cut_off = 1.2f;
```