# [camera](camera.hpp)

Component turning the entity into an in-game camera.

## Members

### frustum

```cpp
putils::rect3f frustum;
```

Specifies the frustum for the camera, i.e. its world coordinates and "size" (FOV for 3D, actual size for 2D).

### pitch, yaw, roll

```cpp
float pitch; // Radians
float yaw; // Radians
float roll; // Radians
```

### near_plane, far_plane

```cpp
float near_plane = 1.f;
float far_plane = 1000.f;
```