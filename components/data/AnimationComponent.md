# [AnimationComponent](AnimationComponent.hpp)

`Component` indicating that an `Entity` should be animated.
Attached to the `Entity` itself.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
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

# [AnimFilesComponent](AnimationComponent.hpp)

`Component` providing a list of animation files to be loaded for a [model](ModelComponent.md) `Entity`.
It is used to populate the `Entity`'s `AnimListComponent`.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)

## Members

### files

```cpp
putils::vector<putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH>, KENGINE_MAX_ANIMATION_FILES> files;
```

List of all animation files.

The maximum length of a file path defaults to 128 and can be adjusted by defining the `KENGINE_ANIMATION_FILE_PATH_LENGTH` macro.

The maximum number of files defaults to 16 and can be adjusted by defining the `KENGINE_MAX_ANIMATION_FILES` macro.

# [AnimListComponent](AnimationComponent.hpp)

`Component` providing a list of animations that can be applied to a given model.
Attached to a [model](ModelComponent.md) `Entity`, this information is shared by all `Entities` using that model.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)

## Anim type

Describes an animation.

```cpp
struct Anim {
    putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH> name;
    float totalTime;
    float ticksPerSecond;
};
```

The maximum length of an animation name defaults to 128 and can be adjusted by defining the `KENGINE_ANIMATION_FILE_PATH_LENGTH` macro.

## Members

### anims

```cpp
putils::vector<Anim, KENGINE_MAX_ANIMATION_FILES> anims;
```

List of all animations that can be applied to a model.

The maximum number of files defaults to 8 and can be adjusted by defining the `KENGINE_MAX_ANIMATION_FILES` macro.