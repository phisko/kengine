# [AnimationComponent](AnimationComponent.hpp)

`Component` indicating that an `Entity` should be animated.
Attached to the `Entity` itself.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)

## Members

### currentAnim

```cpp
unsigned int currentAnim = 0;
```

Index into the [ModelAnimationComponent::animations](ModelAnimationComponent.md) vector of this `Entity`'s [model Entity](ModelComponent.md).

### currentTime

```cpp
float currentTime = 0.f;
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
enum class MoverBehavior {
    UpdateTransformComponent,
    UpdateBones,
    None
};

MoverBehavior positionMoverBehavior = MoverBehavior::UpdateBones;
MoverBehavior rotationMoverBehavior = MoverBehavior::UpdateBones;
MoverBehavior scaleMoverBehavior = MoverBehavior::UpdateBones;
```

Specifies how the animation's movement should be retranscribed:
* by updating the `Entity`'s actual position through the [TransformComponent](TransformComponent.md)
* by updating the bone matrices in the [SkeletonComponent](SkeletonComponent.md)
* not at all