# [attributes](attributes.hpp)

`Meta component` holding a recursive map of the parent component's attributes.

## Usage

It is up to the user to populate this `meta component` for the component types they wish to be able to use it with.

A helper [register_attributes](../helpers/meta/impl/register_attributes.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.
