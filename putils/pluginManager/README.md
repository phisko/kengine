# pluginManager
A simple and re-usable C++ dynamic library abstraction

## EXPORT
Export.hpp contains an `EXPORT` macro which should be added to the prototype of DLL functions to be exported.

#### Example
```
EXPORT void someFunction() {}
```

## Library
The "Library" class family are abstractions to Unix and Windows DLLs. These should not be explitly instantiated, but instead created by `LibraryFactory::make`

### Methods

##### `bool isLoaded()`
Know whether the `Library` was opened correctly.

##### `T execute(const std::string &name, Args &&...args)`
Execute the `name` function, returning T and taking `Args` as parameters.

## LibraryFactory
Has a `make` function which should be used to open DLLs. `make` can either be given a filename ("libssl.so", "ssl.dll") or a "library name" ("ssl").

## PluginManager
Manages a set of DLLs.

### Methods

##### `PluginManager(const std::string &path, bool dir = true)`
Constructor which will open all libraries in `path`. If `dir` is true, `path` is considered to be a directory in which all DLLs will be opened. If `dir` is false, `path` is considered to be a CSV file formatted as "category+;path_to_library". Libraries will therefore be stored in several "categories" for future use.

##### `execute(const std::string &name, Args &&...args)`
Execute, in all DLLs, the `name` function taking `Args` as parameters

##### `std::vector<T> executeWithReturn(const std::string &name, Args &&...args)`
Execute, in all DLLs, the `name` function returning `T` and taking `Args` as parameters.

##### `executeInCategory(const std::string &name, const std::string &category, Args &&...args)`
##### `std::vector<T> executeInCategoryWithReturn(const std::string &name, const std::string &category, Args &&...args)`
