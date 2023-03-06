# [get_bone_matrix](get_bone_matrix.hpp)

```cpp
glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const bone_matrices & matrices, const bone_names & names);
```

Returns the mesh-space matrix for a given bone.