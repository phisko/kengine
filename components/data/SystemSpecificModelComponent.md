# [SystemSpecificModelComponent](SystemSpecificModelComponent.hpp)

```cpp
template<typename Mesh>
struct SystemSpecificModelComponent;
```

`Component` holding system-specific information about a model.

## Members

### meshes

```cpp
std::vector<Mesh> meshes;
```

Collection of the meshes in the model. These are initialized by the system, and typically used by its shaders.