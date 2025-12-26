#pragma once

#include "GLFW/glfw3.h"

#include "Vector.h"

namespace une
{
	class Window
	{
	public:
		Window(int width, int height, const std::string& name);
		~Window();

		Vector2Int GetPosition();
		void SetPosition(const Vector2Int& pos);
		Vector2Int GetSize();
		Vector2 GetScale();
		void SetSize(const Vector2Int& size);
		bool IsFullscreen();
		//Set fullscreen or windowed mode
		void SetFullscreen(bool fullscreen);
		//Return glfwWindowShouldClose for this window
		bool ShouldClose();

		GLFWwindow* glWindow = nullptr;
	private:
		int xPos, yPos;
		int width, height;
		bool fullscreen = false;

		//Return the monitor with greatest window overlap
		//https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
		GLFWmonitor* GetOptimalMonitor();
	};

	extern Window* mainWindow;
	Window* CreateMainWindow(int width, int height, const std::string& name);
}
