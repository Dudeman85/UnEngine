# UI Reference

Most games will need some form of user interface, be it a hud or just menus. For this, UnEngine has the UIElement component which can be attached to any entity to render it as part of the UI.

UI elements have the following properties:
- They are always rendered above all world entities
- They are not affected by the camera's transform or projection
- Their transform is relative to their anchor point and UI canvas, instead of world origin

## UIElement Component
The UIElement component only affects the rendering of the entity. It does this via the canvas and anchor point:
```cpp
struct UIElement
{
	UICanvas* canvas = nullptr;
	//A point inside the canvas in ndc. Transform will be relative to it.
	Vector2 anchor;
};
```
You must set canvas to point to a UICanvas object whose size, position, and rotation will be used to transform the anchor.
The anchor is a point on the canvas in normalized device coordinates (-1, -1) to (1, 1) which the entity's transform will be relative to.

For example, if you want one text to always be 50 units down and 20 units right from the top-left of the screen,
and one sprite to always be 50 units to the left from the center-right you can do the following:
```cpp
//Load the sprite's texture
une::Texture texture("path/to/sprite.png");
une::Font font("path/to/font.otf", 64);
	
//Make the camera
ecs::Entity camera = ecs::NewEntity();
ecs::AddComponent(camera, une::Camera{});
ecs::AddComponent(camera, une::Transform{});
une::CameraSystem::MakeOrtho(camera, 800, 600);

//Make the UI canvas
une::UICanvas canvas;

//Make the UI elements
ecs::Entity uiSprite = ecs::NewEntity();
ecs::AddComponent(uiSprite, une::TextRenderer{.font = &font, .text = "Health: 9000"});
ecs::AddComponent(uiSprite, une::Transform{.position = {20, -50, 0}, .scale = 2});
ecs::AddComponent(uiSprite, une::UIElement{.canvas = &canvas, .anchor = {-1, 1}});
ecs::Entity uiSprite2 = ecs::NewEntity();
ecs::AddComponent(uiSprite2, une::SpriteRenderer{.texture = &texture});
ecs::AddComponent(uiSprite2, une::Transform{.position = {-50, 0, 0}, .scale = 10});
ecs::AddComponent(uiSprite2, une::UIElement{.canvas = &canvas, .anchor = {1, 0}});
```

Note: All children of UI elements should also be UI elements with the same canvas and pivot, otherwise their behavior is undefined.

## Canvas
The canvas basically represents a plane/cube in camera space which the ui elements go into.
However, its scale is based on the window size, where (1, 1) is the same size as the window.

You can use the following methods to access its properties:
```cpp
void SetScale(const Vector3& s);
void SetPosition(const Vector3& pos);
void SetRotation(const Vector3& rot);
void SetNearFar(float near, float far);

Vector3 GetScale() const;
Vector3 GetPosition() const;
Vector3 GetRotation() const;
Vector2 GetNearFar() const;
```