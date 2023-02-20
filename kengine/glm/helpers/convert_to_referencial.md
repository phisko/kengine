# [convert_to_referencial](convert_to_referencial.hpp)

```cpp
putils::point3f convert_to_referencial(const putils::point3f & pos, const glm::mat4 & conversion_matrix) noexcept;
```

Converts `pos` according to `conversion_matrix`. Expressive way of switching from screen-space to world-space (or any other referencial switch).