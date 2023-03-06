# [get_bone_matrix](get_bone_matrix.hpp)

```cpp
glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const skeleton & skeleton, const bone_names & model);
```

Returns the mesh-space matrix for a given bone.