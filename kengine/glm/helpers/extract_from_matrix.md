# [extract_from_matrix](extract_from_matrix.hpp)

```cpp
putils::point3f extract_position(const glm::mat4 & mat) noexcept;
putils::vec3f extract_scale(const glm::mat4 & mat) noexcept;
putils::vec3f extract_rotation(const glm::mat4 & mat) noexcept;
```

Helper functions to extract individual components from a matrix.