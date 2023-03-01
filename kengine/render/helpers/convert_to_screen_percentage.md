# [convert_to_screen_percentage](convert_to_screen_percentage.hpp)

```cpp
putils::rect3f convert_to_screen_percentage(const putils::rect3f & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept;
```

Depending on the `coordinates` of `comp`, converts `rect` to the correct screen percentage.
