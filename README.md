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
Then generate the CMake project for your preferred build tool. Works with CMake > 3.20 and < 4. Tested with GCC, Clang and MSVC, requires C++ 20 or higher.

To build the example projects, set the UNENGINE_BUILD_EXAMPLES=ON CMake option.

### Linux specific
For Debian & pals you need to install a few libraries:
```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
```

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
- [ ] Redo tilemap collision
- [ ] Redo camera for ecs
- [ ] Add an option to change the transform pivot
- [ ] Rethink the transform hierarchy
- [ ] Improve resource loading and management
- [ ] Improve sound loading
- [ ] Better integrate sounds with ecs
- [ ] Fix physics fps bugs
- [ ] Fix shader and renderer destructors
- [ ] Update documentation
### Renderer
- [ ] Redo tilemap rendering
- [x] Fix text resolution scaling
- [ ] Unify model, sprite, text, tilemap, and primitive renderers
- [ ] Fix the sprite scaling
- [ ] Improve model loading and rendering
- [ ] Add transparency to model rendering
- [ ] Finally fix the damn transparency
- [ ] Fix primitive rendering
- [ ] Rethink UI rendering
- [ ] Redo lighting system
- [ ] Add animation support to tilemap
- [ ] Add anti-aliasing
- [ ] Add multi camera support
### New features
- [ ] More robust logging system
- [ ] Add angular momentum
- [ ] Add restraints
- [ ] Component serialization support
- [ ] Serialization of all engine components
- [ ] Add debug tools
  - Camera
  - Frame advance
  - Logging
  - Info
- [ ] Implement enet
- [ ] Add multithreading support
- [ ] Add raycasting
- [ ] Add quaternion support
- [ ] Add extrapolation functions
- [ ] Copy entity function
- [ ] Add TOML or similar config support
- [ ] Add joints

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
