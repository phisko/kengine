# [json_scene_loader](json_scene_loader.hpp)

Component that specifies loading steps for a scene.

## Members

### loading_step

```cpp
//! putils reflect all
struct loading_step {
	string file;
	std::launch policy = std::launch::async;
};
```

A step for loading the scene. `policy` will determine whether it is loaded synchronously.

### temporary_scene

```cpp
loading_step temporary_scene;
```

A path to a JSON file describing entities that will be spawned during loading, and automatically destroyed afterwards.

### model_directory

```cpp
loading_step model_directory;
```

A path to a directory that will be recursively searched for JSON files describing [model entities](model.md). These will all be loaded before creating the scene.

### scene

```cpp
loading_step scene;
```

A path to a JSON file describing entities that will be added to the scene.