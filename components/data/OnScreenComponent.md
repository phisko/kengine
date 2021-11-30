# [OnScreenComponent](OnScreenComponent.hpp)

Parent type for `Components` that need to represent something that will appear on screen.

## Members

### coordinateType

```cpp
enum class CoordinateType {
ScreenPercentage,
Pixels
};

CoordinateType coordinateType = CoordinateType::Pixels;
```

Coordinate system used by the `Entity`.