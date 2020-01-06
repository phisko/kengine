# [PythonStateComponent](PythonStateComponent.hpp)

`Component` that holds a python state in the form of a [pybind11](https://github.com/pybind/pybind11) `scoped_interpreter` and `module`.

## Specs

* Not [reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable (contains pointers)
* Used by the [PySystem](../../systems/PySystem.md)

## Usage

An `Entity` with a `PythonStateComponent` is created by the [PySystem](../../systems/PySystem.md) during initialization. It is then accessible by users to perform any desired operations with python (executing scripts, evaluating python state variables/expressions, or even registering new types and functions).

Note that the `PySystem` already provides a set of helper functions to easily register new types and functions. These helper functions should be preferred over direct manipulation of the `PyStateComponent`.