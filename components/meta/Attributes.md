# [Attributes](Attributes.hpp)

`Meta Component` holding a recursive map of the parent `Component`'s attributes.

## Usage

It is up to the user to populate this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerAttributes](../../helpers/meta/impl/registerAttributes.md) function is provided which takes as a template parameter a set of `Component` types and implements the `Attributes` `meta Component` for them.
