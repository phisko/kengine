# [get_bone_matrix](get_bone_matrix.hpp)

```cpp
glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const data::skeleton & skeleton, const data::model_skeleton & model);
```

Returns the mesh-space matrix for a given bone.