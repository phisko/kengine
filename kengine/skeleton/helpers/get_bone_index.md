# [get_bone_index](get_bone_index.hpp)

```cpp
struct bone_indices {
    unsigned int mesh_index = 0;
    unsigned int bone_index = 0;
};
bone_indices get_bone_index(const entt::registry & r, const char * bone, const model_skeleton & model);
```

Returns the index of the first bone with the given name, along with the index of the mesh it was found in.