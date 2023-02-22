# [system](system.hpp)

System that renders ImGui windows with editing tools for [selected](../../../core/data/selected.md) entities.

## Usage

To make a component appear in the editor, its [edit](../../functions/edit.md) `meta component` must first be implemented. Its [type entity](../../../helpers/get_type_entity.md) must also have a [name](../../../../core/data/name.md).