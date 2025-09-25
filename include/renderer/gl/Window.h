#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <Vector.h>
#include <iostream>

namespace une
{
	static bool OPENGL_INITIALIZED = false;

	static GLFWwindow* mainWindow;

	static int mainWindowWidth = 0;
	static int mainWindowHeight = 0;
	static int mainWindowX = 15;
	static int mainWindowY = 30;
	static bool mainWindowFullscreen = false;

	//Return the monitor with greatest window overlap
	//https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
	GLFWmonitor* GetOptimalMonitor(GLFWwindow* window);

	///Create OpenGL window and context
	GLFWwindow* CreateGLWindow(int width, int height, const char* name, bool fullscreen = false);

	Vector2 GetMainWindowSize();
	void SetFullscreen();
	void SetWindowed();
	void ToggleFullscreen();
}
