# [AnimationComponent](AnimationComponent.hpp)

`Component` indicating that an `Entity` should be animated.
Attached to the `Entity` itself.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### currentAnim

```cpp
unsigned int currentAnim = 0;
```

Index into the `AnimListComponent::allAnims` vector of this `Entity`'s [model info](ModelComponent.md))

##### currentTime

```cpp
float currentTime = 0.f;
```

Current time in the animation.

##### currentTime

```cpp
float speed = 1.f;
```

Speed of the animation.

# [AnimFilesComponent](AnimationComponent.hpp)

`Component` providing a list of animation files to be loaded for an `Entity`.
Attached to the `Entity` itself, used to populate its [model info](ModelComponent.md)'s `AnimListComponent`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### files

```cpp
std::vector<putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH>> files;
```

List of all animation files.
The maximum length of a file path defaults to 128 and can be adjusted by defining the `KENGINE_ANIMATION_FILE_PATH_LENGTH` macro.

# [AnimListComponent](AnimationComponent.hpp)

`Component` providing a list of animations that can be applied to a given model.
Attached to a [model info](ModelComponent.md) `Entity`, shared by all `Entities` using the model info.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Anim type

```cpp
struct Anim {
    putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH> name;
    float totalTime;
    float ticksPerSecond;
};
```

Describes an animation.
Reflectible.
The maximum length of an animation name defaults to 128 and can be adjusted by defining the `KENGINE_ANIMATION_FILE_PATH_LENGTH` macro.

### Members

##### allAnims

```cpp
std::vector<Anim> allAnims;
```

List of all animations that can be applied to a model.