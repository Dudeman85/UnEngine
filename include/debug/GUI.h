#pragma once
#include <functional>
#include <string>
#include <unordered_map>

namespace debug::gui
{
	//Initialize ImGui
	void Init();
	//Clean up resources used by ImGui
	void UnInit();
	//Renders and updates everything
	void Update();

	//Windows
	//Dear ImGui demo
	void DrawDemo();
	//Entity list and world hierarchy
	void DrawEntities();
	//Lists all components and draws inspectors for supported ones
	void DrawInspector();
	//Lists all loaded and loading resources
	void DrawResources();

	//Components
	void TransformInspector();
	void PolygonColliderInspector();
	void RigidbodyInspector();
	void TextRendererInspector();
	void CameraInspector();

	//Function pointers to draw each component inspector by their readable name
	const std::unordered_map<std::string, std::function<void()>> componentDrawFunctions{
		{"Transform", TransformInspector}, {"PolygonCollider", PolygonColliderInspector}, {"TextRenderer", TextRendererInspector},
		{"Rigidbody", RigidbodyInspector}, {"Camera", CameraInspector}
	};

	//GUI window types
	enum class ImWindow { Demo, Entities, Inspector, Resources };
	//What guis are currently visible
	inline std::unordered_map<ImWindow, bool> windowVisibility;
	//Function pointers to draw each gui
	const std::unordered_map<ImWindow, std::function<void()>> windowDrawFunctions{
		{ImWindow::Demo, DrawDemo}, {ImWindow::Entities, DrawEntities}, {ImWindow::Inspector, DrawInspector}, {ImWindow::Resources, DrawResources}
	};

	void EnableWindow(ImWindow window);
	void DisableWindow(ImWindow window);
}
