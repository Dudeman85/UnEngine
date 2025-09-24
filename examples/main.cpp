#include "UnEngine.h"
#include "Vector.h"

int main()
{
    GLFWwindow* window = engine::CreateGLWindow(800, 600, "Window");

    engine::EngineInit();
    engine::Camera cam = engine::Camera(800, 600);
    engine::physicsSystem->gravity = Vector2(0, -400);
    engine::SpriteRenderSystem::SetBackgroundColor(100, 20, 150);

    while (!glfwWindowShouldClose(window))
    {
        double deltaTime = engine::Update(&cam);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}