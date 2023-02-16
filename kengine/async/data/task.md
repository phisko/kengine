# [task](task.hpp)

Component that tracks the progress of an asynchronous task (e.g. loading data from disk on a worker thread). This component does not run the task, and is simply used for tracking purposes. The [async_imgui](../imgui/systems/system.md) system will display running tasks in a debug window.

## Members

### name

```cpp
putils::string<KENGINE_ASYNC_TASK_STRING_MAX_LENGTH> name;
```

The maximum length of this field (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_ASYNC_TASK_STRING_MAX_LENGTH` macro.

### start

```cpp
std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
```

The time at which this task was started.