# [skeleton](skeleton.hpp)

Component that holds transformation matrices for the entity's bones.

## Members

### mesh type

```cpp
struct mesh {
    glm::mat4 bone_mats_bone_space[KENGINE_SKELETON_MAX_BONES]; // Used by shader
    glm::mat4 bone_mats_mesh_space[KENGINE_SKELETON_MAX_BONES]; // Used to get bone matrix in world space
};
```

Represents a single mesh in the model.

`bone_mats_bone_space` holds the matrices for each bone in "bone space", i.e. relative to the bone's identity matrix.

`bone_mats_mesh_space` holds the matrices for each bone in "mesh space", i.e. relative to the entity's position.

### meshes

```cpp
std::vector<mesh> meshes;
```

List of all the meshes comprising the model.