# [model_skeleton](model_skeleton.hpp)

Component listing the bones contained in the [model entity](../../instance/).

## Members

### meshes

```cpp
struct mesh {
    std::vector<std::string> bone_names;
};

std::vector<mesh> meshes;
```

List of all meshes contained in the model.