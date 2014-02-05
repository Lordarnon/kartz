GameFramework
-------------

The GameFramework module provides a simple framework that the "Kartz" example
3D game builds upon.

GameFramework provides functionality including:
- 3D scene graph
- Entities
- Behaviours
- Sky dome ("horizon")
- Paths (including splines)
- Shapes and volumes

GameFramework.mkf cannot be built directly as a library. It must be included
in an MKB project file, using the "subproject GameFramework" directive.
