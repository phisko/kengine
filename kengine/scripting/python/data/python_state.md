# [python_state](python_state.hpp)

Component that holds a python state in the form of a [pybind11](https://github.com/pybind/pybind11) `scoped_interpreter` and `module`.

## Usage

A `python_state` is created by the [python system](../systems/python/python.md) during initialization. It is then accessible by users to perform any desired operations with python (executing scripts, evaluating python state variables/expressions, or even registering new types and functions).

Note that the [python_helper](../helpers/python_helper.md) already provides functions to easily register new types and functions.