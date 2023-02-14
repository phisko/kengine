# [on_screen](on_screen.hpp)

Parent type for components that need to represent something that will appear on screen.

## Members

### coordinate_type

```cpp
enum class coordinate_type {
	screen_percentage,
	pixels
};

coordinate_type coordinates = coordinate_type::pixels;
```

Coordinate system used by the entity.