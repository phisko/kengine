# [lua_state](lua_state.hpp)

Component that holds a Lua state in the form of a [sol::state](https://github.com/ThePhD/sol2) object.

## Usage

A `lua_state` is created by the [lua system](../systems/lua.md) during initialization. It is then accessible by users to perform any desired operations with Lua (executing scripts, evaluating lua state variables/expressions, or even registering new types and functions).

Note that the [lua_helper](../helpers/lua_helper.md) already provides functions to easily register new types and functions.