# [json_scene_loader](json_scene_loader.hpp)

Component that specifies loading steps for a scene.

## Members

### temporary_scene

```cpp
string temporary_scene;
```

A path to a JSON file describing entities that will be spawned before loading, and automatically destroyed afterwards.

### model_directory

```cpp
string model_directory;
```

A path to a directory that will be recursively searched for JSON files describing [model entities](../../model_instance/data/model.md). These will all be loaded asynchronously before loading the scene.

### scene

```cpp
string scene;
```

A path to a JSON file describing entities that will be added to the scene once models have been loaded.