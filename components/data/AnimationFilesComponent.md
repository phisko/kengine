# [AnimationFilesComponent](AnimationFilesComponent.hpp)

`Component` providing a list of animation files to be loaded for a [model](ModelComponent.md) `Entity`.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (holds dynamic pointers)

## Members

### files

```cpp
std::vector<std::string>> files;
```

List of all animation files to be loaded for the model.

