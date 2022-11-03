# [ModelDataComponent](ModelDataComponent.hpp)

Contains the vertices and indices for a model. These are used by graphics systems to load the model onto the GPU. Other systems may also want to access these elements (such as the [RecastSystem](../../systems/recast/RecastSystem.md), which uses them to generate the navmesh).

## Specs

* Not reflectible
* Not serializable
* Initialized by model loading systems (such as the [PolyVoxSystem](../../systems/polyvox/PolyVoxSystem.md)) and processed by graphics systems.

## Members

### meshes

```cpp
struct Mesh {
	struct Buffer {
		size_t nbElements;
		size_t elementSize;
		const void * data;
	};

	Buffer vertices;
	Buffer indices;
	putils::meta::type_index indexType; // unsigned int, unsigned short...
};

std::vector<MeshData> meshes;
```

Represents a model, made up of several meshes, each with their set of vertices and indices.

### free

```cpp
putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE> free = nullptr;
```

Called to release the model data.

The maximum size for the `free` functor defaults to 64 and can be adjusted by defining the `KENGINE_MODEL_LOADER_FUNCTION_SIZE` macro.

### vertexAttributes, vertexSize

```cpp
struct VertexAttribute {
	const char * name;
	size_t offset;
	putils::meta::type_index type;
};

std::vector<VertexAttribute> vertexAttributes;
size_t vertexSize;
```

Information about the vertex type for the model.

### init

```cpp
template<typename VertexType>
void init() noexcept;
```

Initializes the `vertexAttributes` and `vertexSize` fields according to `VertexType`.