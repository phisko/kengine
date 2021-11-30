# [Generate type registration](generateTypeRegistration.py)

This Python script will generate C++ files containing functions to register a set of specified types.

The script will generate a separate cpp file for each type, as well as a `registerTypes.cpp` file and a corresponding header.

All you need to do to have your types registered with the various kengine systems is to include `registerTypes.hpp` and call `registerTypes()` after `kengine::init()`.

## Usage

positional arguments:
  input_files      JSON input files

optional arguments:
  -h, --help       show this help message and exit
  --output OUTPUT  output directory
  --force          overwrite existing files (only use if a component has changed headers)

## Input

The JSON input files should contain the following format:

```
{
        "components": [
                {
                        "type": "kengine::TransformComponent",
                        "header": "data/TransformComponent.hpp"
                },
                ...
        ],
        "types": [
                {
                        "type": "putils::Point3f",
                        "header": "Point.hpp"
                },
                ...
        ]
}
```