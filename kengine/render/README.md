# kengine_render

Components and libraries to render graphical representations of entities.

* [data](data)
	* [asset](data/asset.md): indicates that an entity is the [model](../instance/) for a given asset
	* [camera](data/camera.md): uses an entity as a camera, drawing it to a [viewport](data/viewport.md)
	* [debug_graphics](data/debug_graphics.md): draw debug elements
	* [drawable](data/drawable.md): mark an entity as drawable
	* [god_rays](data/god_rays.md): draw god rays for a [light](data/light.md)
	* [highlight](data/highlight.md): draw a contour around the entity
	* [light](data/light.md): use entities as lights
	* [model_data](data/model_data.md): vertex and index buffers for a model
	* [no_shadow](data/no_shadow.md): disables shadows
	* [on_screen](data/on_screen.md): base class for 2D elements
	* [sky_box](data/sky_box.md): draw a sky box
	* [sprite](data/sprite.md): draw 2D or 3D sprites
	* [text](data/text.md): draw 2D or 3D text
	* [viewport](data/viewport.md): drawable area of a [window](data/window.md)
	* [window](data/window.md): operating system window
* [functions](functions)
	* [appears_in_viewport](functions/appears_in_viewport.md): returns whether the entity should appear in a viewport (or whether the viewport wants an entity to appear in it)
	* [get_entity_in_pixel](functions/get_entity_in_pixel.md): query the GBuffer for which entity is seen in a pixel
	* [get_position_in_pixel](functions/get_position_in_pixel.md): query the GBuffer for which position is seen in a pixel
	* [on_mouse_captured](functions/on_mouse_captured.md): notify systems that the mouse is being captured by a window
* [helpers](helpers)
	* [convert_to_screen_percentage](helpers/convert_to_screen_percentage.md): convert pixel coordinates to screen percentage
	* [entity_appears_in_viewport](helpers/entity_appears_in_viewport.md): check if an entity should appear in a viewport
	* [get_facings](helpers/get_facings.md): get a camera's facings
	* [get_viewport_for_pixel](helpers/get_viewport_for_pixel.md): get the viewport for a given pixel

Sub-libraries:
* [kengine_render_animation](animation): animate entities
* [kengine_render_find_model_by_asset](find_model_by_asset): find an entity's [model](../instance/) based on its [asset](data/asset.md)
* [kengine_render_glfw](glfw): create windows with GLFW
* [kengine_render_kreogl](kreogl): render entities with Kreogl
* [kengine_render_on_click](on_click): notify entities when they are clicked
* [kengine_render_polyvox](polyvox): generate voxel models with Polyvox
* [kengine_render_sfml](sfml): render entities with SFML