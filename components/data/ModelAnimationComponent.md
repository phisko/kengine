# [ModelAnimationComponent](ModelAnimationComponent.hpp)

`Component` holding a list of all animations loaded for a [model](ModelComponent.md) `Entity`, along with functions to extract information from the animations.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (holds dynamic pointers)

## Members

### animations

```cpp
struct Anim {
    std::string name;
    float totalTime;
    float ticksPerSecond;
};

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