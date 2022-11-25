# [matrix_helper](matrix_helper.hpp)

Helper functions for manipulating matrices.

## Members

### to_vec

```cpp
glm::vec3 to_vec(const putils::point3f & pos) noexcept;
```

Converts a `putils::point` to a `glm::vec3`.

### get_position, get_scale, get_rotation

```cpp
putils::point3f get_position(const glm::mat4 & mat) noexcept;
putils::vec3f get_scale(const glm::mat4 & mat) noexcept;
putils::vec3f get_rotation(const glm::mat4 & mat) noexcept;
```

Extracts the required components from a transformation matrix.

### convert_to_referencial

```cpp
putils::point3f convert_to_referencial(const putils::point3f & pos, const glm::mat4 & conversion_matrix) noexcept;
```

Converts `pos` to the referencial given by `conversion_matrix`.

### get_model_matrix

```cpp
glm::mat4 get_model_matrix(const data::transform & transform, const data::transform * model) noexcept;
```

Generates a model matrix for an entity, applying any transformations it may have inherited from its `model`.