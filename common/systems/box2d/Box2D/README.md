Box2D for C++
=============

[Box2D](https://github.com/erincatto/box2d) is a 2D physics engine mostly targeted towards games.

For help regarding Box2D's features, please visit the [official website](http://www.box2d.org/), where you can also find the [official forum](http://box2d.org/forum/). For problems related to this port, you may contact me at eXpl0it3r@sfml-dev.org or create an issue on the [tracker](https://github.com/eXpl0it3r/Box2D/issues).

About this Port
---------------

The main motivation behind this port is to provide a true C++ version of Box2D as opposed to the current C-like Box2D and make the CMake build system more flexible.

**Notice:** Since this port will be kept in sync with the [Git repository](https://github.com/erincatto/box2d), the master branch will receive forced pushes.

Here, I'll list the changes and the progress:

* [X] Restructure the directory layout
* [X] Update the CMake build system
* [X] Add a true namespace and remove the `b2` prefixes
* [X] Replace preprocessor constants with C++ constants
* [X] Use C++ headers instead of C headers
* [X] Update the "Hello World" example
* [X] Update the "Testbed" example
* [X] Rebase onto the new [Git repository](https://github.com/erincatto/box2d)
* [ ] Adjust the build system to properly work with Visual Studios
* [ ] Add Doxygen documentation generation to CMake

Hints for the Testbed
---------------------

Some test bed commands are:	
* 'r' to reset the current test
* SPACE to launch a bomb
* arrow keys to pan
* 'x' and 'z' to zoom in/out
* use the mouse to click and drag objects

License
-------

As the original Box2D this port is released under the zlib/libpng license, see the [LICENSE](LICENSE) file.

Credits
-------

Erin Catto

http://www.box2d.org

https://github.com/erincatto/box2d
