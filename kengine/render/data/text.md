# [text](text.hpp)

```cpp
struct text;
struct text_2d : text, on_screen;
struct text_3d : text;
```

Components that render 2D or 3D text.

## Members

### text, font, font_size

```cpp
using string = putils::string<KENGINE_FONT_PATH_MAX_LENGTH>;

string text;
string font;
size_t font_size;
```

The text to be displayed, and the font it should be displayed in.

The maximum length of these fields (stored as [putils::strings](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 128, and can be adjusted by defining the `KENGINE_FONT_PATH_MAX_LENGTH` macro.

### color

```cpp
putils::normalized_color color;
```

Color in which the text should be displayed. Stored as a [putils::normalized_color](https://github.com/phisko/putils/blob/master/putils/color.md).

### alignment

```cpp
enum class alignment_type {
    left,
    center,
    right
};

alignment_type alignment = alignment_type::center;
```