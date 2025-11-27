#include "glad/gl.h"

#include "renderer/gl/Window.h"
#include "debug/Logging.h"

namespace une
{
	Window* mainWindow;

	Window::Window(int width, int height, const std::string& name)
	{
		//Initialize GLFW and set it to require OpenGL 3.30
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//Enable 4xMSAA
		glfwWindowHint(GLFW_SAMPLES, 4);

		//Create window object
		glWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		if (glWindow == nullptr)
		{
			const char* desc;
			glfwGetError(&desc);
			glfwTerminate();
			debug::LogError("Failed to create GLFW window: " + std::string(desc));
			throw std::runtime_error("Failed to create GLFW window: " + std::string(desc));
		}
		glfwMakeContextCurrent(glWindow);
		if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
		{
			debug::LogError("Failed to initialize GLAD");
			throw std::runtime_error("Failed to initialize GLAD");
		}

		//Set the resize window callback function
		glfwSetFramebufferSizeCallback(glWindow, [](GLFWwindow* window, int w, int h)
			{
				glViewport(0, 0, w, h);
			}
		);

		glViewport(0, 0, width, height);
		debug::LogInfo("Successfully initialized GLFW");
	}

	Window::~Window()
	{
		glfwDestroyWindow(glWindow);
		glfwTerminate();
	}

	Vector2Int Window::GetPosition()
	{
		glfwGetWindowPos(glWindow, &xPos, &yPos);
		return Vector2Int(xPos, yPos);
	}
	void Window::SetPosition(const Vector2Int& pos)
	{
		xPos = pos.x;
		yPos = pos.y;
		glfwSetWindowPos(glWindow, xPos, yPos);
	}
	Vector2Int Window::GetSize()
	{
		glfwGetWindowSize(glWindow, &width, &height);
		return Vector2Int(width, height);
	}
	void Window::SetSize(const Vector2Int& size)
	{
		width = size.x;
		height = size.y;
		glfwSetWindowSize(glWindow, width, height);
	}
	bool Window::IsFullscreen()
	{
		return fullscreen;
	}

	//Set fullscreen or windowed mode
	void Window::SetFullscreen(bool fullscreen)
	{
		this->fullscreen = fullscreen;
		if (fullscreen)
		{
			//Save windowed pos
			glfwGetWindowPos(glWindow, &xPos, &yPos);
			glfwGetWindowSize(glWindow, &width, &height);

			GLFWmonitor* monitor = GetOptimalMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			int x, y, w, h;
			glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
			glfwSetWindowMonitor(glWindow, monitor, x, y, w, h, mode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(glWindow, nullptr, xPos, yPos, width, height, 60);
		}
	}

	//Return glfwWindowShouldClose for this window
	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(glWindow);
	}

	//Return the monitor with greatest window overlap
	//https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
	GLFWmonitor* Window::GetOptimalMonitor()
	{
		int nmonitors, i;
		int wx, wy, ww, wh;
		int mx, my, mw, mh;
		int overlap, bestoverlap;
		GLFWmonitor* bestmonitor;
		GLFWmonitor** monitors;
		const GLFWvidmode* mode;

		bestoverlap = 0;
		bestmonitor = NULL;

		glfwGetWindowPos(glWindow, &wx, &wy);
		glfwGetWindowSize(glWindow, &ww, &wh);
		monitors = glfwGetMonitors(&nmonitors);

		for (i = 0; i < nmonitors; i++) {
			mode = glfwGetVideoMode(monitors[i]);
			glfwGetMonitorPos(monitors[i], &mx, &my);
			mw = mode->width;
			mh = mode->height;

			overlap =
				std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
				std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

			if (bestoverlap < overlap) {
				bestoverlap = overlap;
				bestmonitor = monitors[i];
			}
		}

		return bestmonitor;
	}

	Window* CreateMainWindow(int width, int height, const std::string& name)
	{
		mainWindow = new Window(width, height, name);
		return mainWindow;
	}
}
