# [animation](animation.hpp)

Component that plays an animation on the entity.

## Members

### current_anim

```cpp
unsigned int current_anim = 0;
```

Index into the [model_animation::animations](model_animation.md) vector of this entity's [model entity](../../model/).

### current_time

```cpp
float current_time = 0.f;
```

Current time in the animation.

### speed

```cpp
float speed = 1.f;
```

Speed of the animation.

### loop

```cpp
bool loop = true;
```

Whether the animation should loop or not.

### Mover behaviors

```cpp
enum class mover_behavior {
    update_transform_component,
    update_bones,
    none
};

mover_behavior position_mover_behavior = mover_behavior::update_bones;
mover_behavior rotation_mover_behavior = mover_behavior::update_bones;
mover_behavior scale_mover_behavior = mover_behavior::update_bones;
```

Specifies how to apply the animation's movement:
* by updating the entity's actual position through its [transform](../../core/data/transform.md)
* by updating the bone matrices in its [skeleton](../../skeleton/data/bone_matrices.md)
* not at all