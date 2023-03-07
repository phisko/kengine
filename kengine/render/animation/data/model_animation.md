# [model_animation](model_animation.hpp)

Component holding a list of all animations loaded for a [model entity](../../../model/). `model_animation` also exposes functions to extract information from the animations.

## Members

### animations

```cpp
struct anim {
    std::string name;
    float total_time;
    float ticks_per_second;
};

std::vector<anim> animations;
```

List of all animations that were loaded from [animation_files::files](animation_files.md).

### extracted_motion_getter type

```cpp
using extracted_motion_getter = putils::function<putils::point3f(entt::entity e, size_t anim, float time), KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE>;
```

Function that, given an animation index and a time, returns one of the motion components from that animation until that time, scaled according to `e`'s [transform](../../core/data/transform.md).

The maximum size for this functor defaults to 64 and can be adjusted by defining the `KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE` macro.

### Motion extracting functions

```cpp
extracted_motion_getter get_animation_movement_until_time;
extracted_motion_getter get_animation_rotation_until_time;
extracted_motion_getter get_animation_scaling_until_time;
```

Functors that will return, respectively, the movement, rotation and scaling components of a given animation until a given time.