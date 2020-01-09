# [TextComponent](TextComponent.hpp)

`Component` that lets an `Entity` be used to render 2D or 3D text.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### text, font, fontSize

```cpp
using string = putils::string<KENGINE_FONT_PATH_MAX_LENGTH>;

string text;
string font;
size_t fontSize;
```

The text to be displayed, and the font it should be displayed in.

The maximum length of these fields (stored as [putils::strings](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 128, and can be adjusted by defining the `KENGINE_FONT_PATH_MAX_LENGTH` macro.

### color

```cpp
putils::NormalizedColor color;
```

Color in which the text should be displayed. Stored as a [putils::NormalizedColor](https://github.com/phisko/putils/blob/master/Color.md).

### alignment

```cpp
enum class Alignment {
    Left,
    Center,
    Right
};

Alignment alignment = Alignment::Center;
```