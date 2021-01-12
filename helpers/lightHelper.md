# [lightHelper](lightHelper.hpp)

Helper functions for manipulating [light Entities](../components/data/LightComponent.md).

## Members

### getCSMCascadeEnd

```cpp
float getCSMCascadeEnd(const DirLightComponent & light, size_t csmIndex) noexcept;
```

Returns the cascade end distance for a given index of `light`'s Cascaded Shadow Map.

### getCSMLightSpaceMatrix

```cpp
glm::mat4 getCSMLightSpaceMatrix(const DirLightComponent & light, const putils::gl::Program::Parameters & params, size_t csmIndex) noexcept;
```

Returns the light space matrix for a given cascade of a directional light's [Cascaded Shadow Map](https://docs.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps).

### getLightSpaceMatrix

```cpp
glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, const putils::gl::Program::Parameters & params) noexcept;
```

Returns the light space matrix for a spot light.

### getRadius

```cpp
float getRadius(const PointLightComponent & light) noexcept;
```

Returns an estimated radius for a point light.
