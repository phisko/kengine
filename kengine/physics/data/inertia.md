# [physics](inertia.hpp)

Component that moves the entity.

## Members

### movement

```cpp
putils::vec3f movement;
```
Indicates the movement vector for the entity in the current frame.

### yaw, pitch, roll

```cpp
float yaw = 0.f;
float pitch = 0.f;
float roll = 0.f;
```

Radians to turn in the current frame.

### mass

```cpp
float mass = 1.f;
```

### changed

```cpp
bool changed = false;
```
Indicates that the component was modified by a "user" system (and not the actual physics system) and should be read by the physics system.