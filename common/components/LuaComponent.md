# LuaComponent

`Component` that specifies the list of lua scripts to be run for the `GameObject` it is attached to.

Inherits from [putils::Reflectible](../../putils/reflection/Reflectible.md).

In all the following functions, a `script` is the path to a file containing a script, NOT THE ACTUAL SCRIPT CODE.

### Members

##### Constructor

```
LuaComponent(const std::vector<std::string> &scripts = {});
```

##### attachScript

```
void attachScript(std::string_view file) noexcept;
```

##### removeScript

```
void removeScript(std::string_view file) noexcept;
```

##### getScripts

```
const std::vector<std::string> &getScripts() const noexcept;
```
