 # [DebugGraphicsComponent](DebugGraphicsComponent.hpp)

`Component` that provides debug information to be drawn for an `Entity`.

Debug information can be:
* text
* line
* sphere
* box

The maximum length of the debug text and font (stored as [putils::strings](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH` macro.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### Constructors

* Text
```cpp
DebugGraphicsComponent(const char * text, unsigned int textSize, const char * font, const putils::Point3f & startPos, unsigned int color);
```

* Line
```cpp
DebugGraphicsComponent(const putils::Point3f & startPos, const putils::Point3f & endPos, float thickness, unsigned int color);
```

* Sphere
```cpp
DebugGraphicsComponent(const putils::Point3f & center, float radius, unsigned int color);
```

* Box
```cpp
DebugGraphicsComponent(const putils::Rect3f & box, unsigned int color);
```