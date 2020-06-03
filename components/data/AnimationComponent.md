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

Index into the `AnimListComponent::anims` vector of this `Entity`'s [model Entity](ModelComponent.md).

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

# [ModelAnimationComponent](AnimationComponent.hpp)

`Component` providing a list of animation files to be loaded for a [model](ModelComponent.md) `Entity`. Once processed by an animation system, it holds a list of all the animations that were loaded, along with functions to extract information from the animations.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (holds dynamic pointers)

## Members

### files

```cpp
std::vector<std::string>> files;
```

List of all animation files to be loaded for the model.

### Anim type

Describes an animation.

```cpp
struct Anim {
    std::string name;
    float totalTime;
    float ticksPerSecond;
};
```

### animations

```cpp
std::vector<Anim> animations;
```

List of all animations that were loaded from `files`.

### ExtractedMotionGetter type

```cpp
using ExtractedMotionGetter = putils::function<putils::Point3f(const Entity & e, size_t anim, float time), KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE>;
```

Function that, given an animation index and a time, returns one of the motion components from that animation until that time, scaled according to `e`'s [TransformComponent](TransformComponent.md).

The maximum size for this functor defaults to 64 and can be adjusted by defining the KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE macro.

### Motion extracting functions

```cpp
ExtractedMotionGetter getAnimationMovementUntilTime;
ExtractedMotionGetter getAnimationRotationUntilTime;
ExtractedMotionGetter getAnimationScalingUntilTime;
```

Functors that will return, respectively, the movement, rotation and scaling components of a given animation until a given time.