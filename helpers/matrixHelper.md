# [matrixHelper](matrixHelper.hpp)

Helper functions for manipulating matrices.

## Members

### toVec

```cpp
glm::vec3 toVec(const putils::Point3f & pos) noexcept;
```

Converts a `putils::Point` to a `glm::vec3`.

### getPosition, getScale, getRotation

```cpp
putils::Point3f getPosition(const glm::mat4 & mat) noexcept;
putils::Vector3f getScale(const glm::mat4 & mat) noexcept;
putils::Vector3f getRotation(const glm::mat4 & mat) noexcept;
```

Extracts the required components from a transformation matrix.

### convertToReferencial

```cpp
putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix) noexcept;
```

Converts `pos` to the referencial given by `conversionMatrix`.

### getModelMatrix

```cpp
glm::mat4 getModelMatrix(const TransformComponent & transform, const TransformComponent * model) noexcept;
```

Generates a model matrix for an `Entity`, applying any transformations it may have inherited from its `model`.