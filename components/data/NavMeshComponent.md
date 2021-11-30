# [NavMeshComponent](NavMeshComponent.hpp)

`Component` for a [model Entity](ModelComponent.md) holding the information required to build a navmesh for the given model.

# GetPath

```cpp
namespace functions {
    namespace GetPathImpl {
        static constexpr char PathName[] = "NavMeshComponentPath";
        using Path = putils::vector<putils::Point3f, KENGINE_NAVMESH_MAX_PATH_LENGTH, PathName>;
    }

    struct GetPath : BaseFunction<
        GetPathImpl::Path (const Entity & environment, const putils::Point3f & start, const putils::Point3f & end)
        // `environment` is the entity instantiating the `model Entity` this component is attached to
    > {};
}
```

`Function Component` attached to a model Entity that will return the path from `start` to `end` when navigating in the `environment` Entity, which is an instance of the model.