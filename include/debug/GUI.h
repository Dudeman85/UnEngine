#pragma once
#include <functional>
#include <unordered_map>

#include "imgui.h"

namespace debug::gui
{
	enum class ImWindow { Demo, Entities, Components };

	//What guis are currently visible
	inline std::unordered_map<ImWindow, bool> windowVisibility;

	//Initialize ImGui
	void Init();
	//Clean up resources used by ImGui
	void UnInit();
	//Renders and updates everything
	void Update();

	void EnableWindow(ImWindow window);
	void DisableWindow(ImWindow window);

	//Windows
	//Dear ImGui demo
	void DrawDemo();
	//Entity list and world hierarchy
	void DrawEntities();
	//Lists all components and draws inspectors for supported ones
	void DrawComponents();

	//Components
	//Transform component editor
	void DrawTransform();

	const std::unordered_map<ImWindow, std::function<void()>> windowDrawFunctions{
		{ImWindow::Demo, DrawDemo}, {ImWindow::Entities, DrawEntities}, {ImWindow::Components, DrawComponents}
	};
}
