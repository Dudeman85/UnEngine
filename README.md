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
After cloning the repo update the submodules:
```bash
git submodule update --init
```
Then generate the CMake project for your preferred build tool. Works with CMake > 3.20 and < 4. Tested with GCC, Clang and MSVC, requires C++17 or higher.

To build the example projects, set the UNENGINE_BUILD_EXAMPLES=ON CMake option.

### Linux specific
For Debian & pals you need to install a few libraries:
```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
```

## TODO
## Required
### Improvements
- [x] Move to new repo
- [x] Add linux build compatibility
- [x] Update all dependencies
- [x] Refactor to h + cpp
- [x] Unify model, sprite, text, tilemap, and primitive renderers
  - [x] Improve tilemap rendering
  - [x] Fix sprite scaling
- [ ] Improve tilemap collision
- [ ] Redo camera for ecs
- [ ] Redo UI system
- [ ] Redo window and context system

### New Features
- [x] Implement enet (was already implemented)
- [x] Component serialization support

## High Priority
### Improvements
- [x] Update the vector library
- [ ] Expand and improve the timing system
- [ ] Improve resource loading and management
- [ ] Add an option to change the transform pivot
- [ ] Improve animation system
- [ ] Rethink the transform hierarchy
- [x] Fix text resolution scaling
- [ ] Fix sound bugs
- [ ] Better integrate sounds with ecs

### New features
- [ ] Implement UPnP for automatic port forwarding
- [ ] Add animation support to tilemap
- [ ] Add Hexagonal tilemap support
- [x] More robust logging system
- [ ] Add TOML or similar config support
- [ ] Add multithreading support
- [ ] Add multi camera support

## Low Priority
### Improvements
- [ ] Improve model loading and rendering
- [ ] Add transparency to model rendering
- [ ] Redo lighting system
- [ ] Fix physics fps bugs
- [ ] Fix primitive rendering
- [ ] Update documentation
- [ ] Fix shader and renderer destructors
- [ ] Serialization of all engine components

### New Features
- [ ] Create a particle system
- [ ] Add 3D animation support
- [ ] Add debug tools
  - [ ] Camera
  - [ ] Frame advance
  - [x] Logging
  - [ ] Info
- [ ] Copy entity function
- [ ] Add angular momentum
- [ ] Add restraints
- [ ] Add joints
- [ ] Add raycasting
- [ ] Add quaternion support
- [ ] Add anti-aliasing
- [ ] Add extrapolation functions

### Idea for new UI system
Rip off unity again, we have a ui component and ui canvas:
```
struct UIComponent
{
    UICanvas*
    ...
}
struct UICanvas // not a component
{
    enabled
    size //relative to viewport
    offset //relative to viewport
    rotation 
}
```
All ui elements will be transformed using a canvas instead of the camera's view, still have to figure out what to do about projection, probably just use ortho.
This means the UI elements will use standard coordinates relative to the canvas instead of ndc like it was. Makes it easier to position things, make menus, simplify code, and hide/show things, also unifies ui element implementation instead of it being renderer specific.
