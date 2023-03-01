# [get_facings](get_facings.hpp)

```cpp
struct facings {
    putils::vec3f front;
    putils::vec3f right;
    putils::vec3f up;
};

facings get_facings(const data::camera & camera) noexcept;
```

Returns the facing vectors for `camera`.
