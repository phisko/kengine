# [PythonStateComponent](PythonStateComponent.hpp)

`Component` that holds a python state in the form of a [pybind11](https://github.com/pybind/pybind11) `scoped_interpreter` and `module`.

## Specs

* Not [reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (contains pointers)
* Used by the [PythonSystem](../../systems/python/PythonSystem.md)

## Usage

An `Entity` with a `PythonStateComponent` is created by the [PythonSystem](../../systems/python/PythonSystem.md) during initialization. It is then accessible by users to perform any desired operations with python (executing scripts, evaluating python state variables/expressions, or even registering new types and functions).

Note that the [pythonHelper](../../helpers/pythonHelper.md) already provides functions to easily register new types and functions.