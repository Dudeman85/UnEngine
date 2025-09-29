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
- [x] Refactor to h + cpp
- [ ] Update the vector library
- [ ] ~~Update to CMake 4~~
### Improvements
- [ ] Better integrate tilemap with ecs
- [ ] Redo camera
- [ ] Add an option to change the transform pivot
- [ ] Rethink the transform hierarchy
- [ ] Improve resource loading and management
- [ ] Improve sound loading
- [ ] Better integrate sounds with ecs
- [ ] Fix physics fps bugs
- [ ] Update documentation
### Renderer
- [ ] Redo tilemap rendering
- [ ] Unify model, sprite, text, tilemap, and primitive renderers
- [ ] Fix the sprite scaling
- [ ] Improve model loading and rendering
- [ ] Add transparency to model rendering
- [ ] Finally fix the damn transparency
- [ ] Fix primitive rendering
- [ ] Rethink UI rendering
- [ ] Redo lighting system
- [ ] Add anti-aliasing
- [ ] Add multi camera support
### New features
- [ ] Add angular momentum
- [ ] Add restraints
- [ ] Component serialization support
- [ ] Serialization of all engine components
- [ ] More robust logging system
- [ ] Add debug tools
  - Camera
  - Frame advance
  - Logging
  - Info
- [ ] Implement enet
- [ ] Add raycasting
- [ ] Add quaternion support
- [ ] Add extrapolation functions
- [ ] Copy entity function
- [ ] Add TOML or similar config support
- [ ] Add joints
