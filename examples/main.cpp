#include "UnEngine.h"
#include "Vector.h"

int main()
{
    GLFWwindow* window = une::CreateGLWindow(800, 600, "Window");

    une::EngineInit();
    une::Camera cam = une::Camera(800, 600);
    une::physicsSystem->gravity = Vector2(0, -400);
    une::renderer::SetBackgroundColor(une::Color(100, 20, 150));

    une::Texture texture("../../../examples/assets/strawberry.png");
    une::Texture transparentTexture("../../../examples/assets/Transparent.png");
    une::Primitive square = une::Primitive::Rectangle();

    ecs::Entity e0 = ecs::NewEntity();
    ecs::AddComponent(e0, une::SpriteRenderer{.texture = &texture});
    ecs::AddComponent(e0, une::Transform{.position = {-50, 30, -10}, .scale = 150});
    ecs::Entity e1 = ecs::NewEntity();
    ecs::AddComponent(e1, une::SpriteRenderer{.texture = &transparentTexture});
    ecs::AddComponent(e1, une::Transform{.position = {0, 0, -20}, .scale = 100});
    ecs::Entity e2 = ecs::NewEntity();
    ecs::AddComponent(e2, une::SpriteRenderer{.texture = &transparentTexture});
    ecs::AddComponent(e2, une::Transform{.position = {10, 10, -10}, .scale = 100});
    ecs::AddComponent(e2, une::PrimitiveRenderer{.primitive = &square, .color = une::Color(250, 50, 250, 100)});

    while (!glfwWindowShouldClose(window))
    {
        double deltaTime = une::Update(&cam);

        if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            une::TransformSystem::Translate(e2, 2, 0, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
            une::TransformSystem::Translate(e2, -2, 0, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_UP))
        {
            une::TransformSystem::Translate(e2, 0, 2, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN))
        {
            une::TransformSystem::Translate(e2, 0, -2, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
        {
            une::TransformSystem::Translate(e2, 0, 0, 1);
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
        {
            une::TransformSystem::Translate(e2, 0, 0, -1);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}