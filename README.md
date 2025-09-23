# UnEngine

UnEngine is a primarily 2D game engine made from scratch. 
It is built on a custom implementation of [Entity Component System (ECS)](doc/ECS%20Reference.md).

## Features:
- [ECS](doc/ECS%20Reference.md)
- [Transform](doc/Transform%20Reference.md)
- [Sprite Rendering](doc/Sprite%20Reference.md)
- [Model Rendering](todo)
- [Text Rendering](doc/TextRender%20Reference.md)
- [Tilemap Rendering](doc/Tiled%20Reference.md)
- [Physics & Collision](doc/Physics%20Reference.md)
- [Spritesheet Animator](doc/Sprite%20Reference.md)
- [Audio](doc/Audio%20Reference.md)

## Building
### Windows
### Linux
For Debian you need to install a few libraries:

```sudo apt install libwayland-dev libxkbcommon-dev xorg-dev```

## TODO
### General
- [x] Move to new repo
- [x] Add linux build compatibility
- [x] Update all dependencies
- [ ] Refactor to h + cpp
- [ ] Update the vector library
- [ ] ~~Update to CMake 4~~
### Improvements
- [ ] Rethink the transform hierarchy
- [ ] Add an option to change the transform pivot
- [ ] Improve resource loading and management
- [ ] Improve sound loading
- [ ] Better integrate sounds with ecs
- [ ] Better integrate tilemap with ecs
- [ ] Redo camera
### Renderer
- [ ] Unify model, sprite, and primitive renderers
- [ ] Fix the sprite scaling
- [ ] Add multi camera support
- [ ] Redo lighting system
- [ ] Finally fix the damn transparency
- [ ] Fix primitive rendering
### New features
- [ ] Add quaternion support
- [ ] Add extrapolation functions
- [ ] More robust logging system
- [ ] Implement enet
- [ ] Copy entity function
- [ ] Component serialization support
- [ ] Serialization of all engine components
- [ ] Add raycasting
- [ ] Add angular momentum
- [ ] Add restraints
- [ ] Add joints
- [ ] Add TOML or similar config support
- [ ] Add debug tools
  - Camera
  - Frame advance
  - Logging
  - Info

