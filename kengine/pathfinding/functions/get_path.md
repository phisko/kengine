# [get_path](get_path.hpp)

```cpp
namespace get_path_impl {
	static constexpr char string_name[] = "get_path_string";
	using path = putils::vector<putils::point3f, KENGINE_NAVMESH_MAX_PATH_LENGTH, string_name>;
}

struct get_path : base_function<
	get_path_impl::path (entt::handle environment, const putils::point3f & start, const putils::point3f & end)
> {};
```

`Function component` attached to a `model entity` that will return the path from `start` to `end` when navigating in `environment`, which is an instance of the model.
