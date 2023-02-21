# [get_model_matrix](get_model_matrix.hpp)

```cpp
glm::mat4 get_model_matrix(const core::transform & transform, const core::transform * model) noexcept;
```

Generates a model matrix for an entity, applying any transformations it may have inherited from its `model`.