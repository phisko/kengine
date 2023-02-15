# [nav_mesh](nav_mesh.hpp)

Component that specifies how to build a navmesh for the [model entity](../../model_instance/data/model.md).

# get_path

```cpp
namespace functions {
    namespace get_path_impl {
        static constexpr char PathName[] = "NavMeshComponentPath";
        using Path = putils::vector<putils::point3f, KENGINE_NAVMESH_MAX_PATH_LENGTH, PathName>;
    }

    struct get_path : base_function<
        get_path_impl::Path (const Entity & environment, const putils::point3f & start, const putils::point3f & end)
        // `environment` is the entity instantiating the `model Entity` this component is attached to
    > {};
}
```

`Function component` attached to a `model entity` that will return the path from `start` to `end` when navigating in the `environment` entity, which is an instance of the model.