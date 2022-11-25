# [transform](transform.hpp)

Component that provides the entity's position and size. 

## Members

### bounding_box

```cpp
putils::rect3f bounding_box{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
```

The `Entity`'s position is stored as `bounding_box.position`, and its scale as `bounding_box.size`.

### pitch, yaw, roll

```cpp
float pitch = 0; // Radians
float yaw = 0; // Radians
float roll = 0; // Radians
```