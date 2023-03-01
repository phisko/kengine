# [model_data](model_data.hpp)

Component holding the vertices and indices for a [model entity](model.md). These are used by graphics systems to load the model onto the GPU. Other systems may also want to access these elements (such as the [recast system](../../pathfinding/recast/systems/system.md), which uses them to generate the navmesh).

## Members

### meshes

```cpp
struct mesh {
	struct buffer {
		size_t nb_elements;
		size_t element_size;
		const void * data;
	};

	buffer vertices;
	buffer indices;
	putils::meta::type_index index_type; // unsigned int, unsigned short...
};

std::vector<mesh> meshes;
```

Represents a model, made up of several meshes, each with their set of vertices and indices.

### free

```cpp
putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE> free = nullptr;
```

Called to release the model data.

The maximum size for the `free` functor defaults to 64 and can be adjusted by defining the `KENGINE_MODEL_LOADER_FUNCTION_SIZE` macro.

### vertex_attributes, vertex_size

```cpp
struct vertex_attribute {
	const char * name;
	size_t offset;
	putils::meta::type_index type;
};

std::vector<vertex_attribute> vertex_attributes;
size_t vertex_size;
```

Information about the vertex type for the model.

### init

```cpp
template<typename VertexType>
void init() noexcept;
```

Initializes the `vertex_attributes` and `vertex_size` fields according to `VertexType`.