#include "UnEngine.h"
#include "Vector.h"

int main()
{
    GLFWwindow* window = une::CreateGLWindow(800, 600, "Window");

    une::EngineInit();
    une::Camera cam = une::Camera(800, 600);
    une::physicsSystem->gravity = Vector2(0, -400);
    une::renderer::SetBackgroundColor(100, 20, 150);

    une::Texture texture("../../../examples/assets/strawberry.png");

    ecs::Entity e1 = ecs::NewEntity();
    ecs::AddComponent(e1, une::SpriteRenderer{.texture = &texture});
    ecs::AddComponent(e1, une::Transform{.position = {0, 0, -10}, .scale = 100});
    ecs::Entity e2 = ecs::NewEntity();
    ecs::AddComponent(e2, une::SpriteRenderer{.texture = &texture});
    ecs::AddComponent(e2, une::Transform{.position = {10, 10, -10}, .scale = 100});

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
            une::TransformSystem::Translate(e2, 0, 0, 2);
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
        {
            une::TransformSystem::Translate(e2, 0, 0, -2);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}