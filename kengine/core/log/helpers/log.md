# [log](log.hpp)

```cpp
void log(const entt::registry & r, log_severity severity, const char * category, const char * message) noexcept;
```

For each entity with the [on_log](../functions/on_log.md) `function component`:
* check if it has a [severity_control](../data/severity_control.md)
	* if it does, check whether the provided arguments pass the control
		* if they do, call its `on_log` with the provided arguments
	* if it does not, call its `on_log` with the provided arguments