# UI Reference

Most games will need some form of user interface, be it a hud or just menus. For this, UnEngine has the UIElement component which can be attached to any entity to render it as part of the UI.
UI elements have the following properties:
- They are always rendered above all world entities
- They are not affected by the camera's transform or projection
- Their transform is relative to the anchor point and UI canvas (defined in UIElement) instead of world origin

## UIElement Component
The UIElement component only affects the rendering of the entity. It only has a canvas and anchor point:
```cpp
struct UIElement
{
	UICanvas* canvas = nullptr;
	//A point inside the canvas in ndc. Transform will be relative to it.
	Vector2 anchor;
};
```
You must set canvas to point to a UICanvas object whose size, position, and rotation will be the basis of transforming the UIElement. 
The anchor is a point on the canvas in normalized device coordinates (-1, -1) to (1, 1) which the entity's transform will be relative to.

For example, if you want a sprite to always be 50 units from the top left corner of the screen no matter the size of the camera or window, you can do the following:
```cpp
//Load the sprite's texture
une::Texture texture("path/to/spite.png");
	
//Make the camera
ecs::Entity camera = ecs::NewEntity();
ecs::AddComponent(camera, une::Camera{.viewport = {0, 0, 1, 1}});
ecs::AddComponent(camera, une::Transform{.position = {0, 0, 3000}});
une::CameraSystem::MakeOrtho(camera, 800, 600);

//Make the UI canvas
une::UICanvas canvas(800, 600);

//Make the UI sprite
ecs::Entity uiSprite = ecs::NewEntity();
ecs::AddComponent(uiSprite, une::SpriteRenderer{.texture = &texture});
ecs::AddComponent(uiSprite, une::Transform{.position = {50, -50, 0}});
ecs::AddComponent(uiSprite, une::UIElement{.canvas = &canvas, .anchor = {-1, 1}});
```

