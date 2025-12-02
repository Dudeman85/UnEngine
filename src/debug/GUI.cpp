#include "debug/GUI.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ECS.h"
#include "UnEngine.h"
#include "renderer/gl/Window.h"

namespace debug::gui
{
	//Initialize ImGui
	void Init()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		//Setup backends
		ImGui_ImplGlfw_InitForOpenGL(une::mainWindow->glWindow, true);
		ImGui_ImplOpenGL3_Init();
	}

	//Clean up resources used by ImGui
	void UnInit()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	//Renders and updates everything
	void Update()
	{
		//Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		for (std::pair<ImWindow, bool> w : windowVisibility)
		{
			if (w.second)
			{
				windowDrawFunctions.at(w.first)();
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EnableWindow(ImWindow window)
	{
		windowVisibility[window] = true;
	}
	void DisableWindow(ImWindow window)
	{
		windowVisibility[window] = false;
	}

	//Windows

	ecs::Entity selectedEntity;
	std::unordered_map<ecs::Entity, bool> entitySelection;

	//Dear ImGui demo
	void DrawDemo()
	{
		ImGui::ShowDemoWindow(&windowVisibility[ImWindow::Demo]);
	}

	//Recursively draws entity and all its children to hierarchy
	void DrawHierarchyEntity(ecs::Entity entity, int depth)
	{
		std::string text;
		for (int i = 0; i < depth; ++i)
			text.append(" ");
		text.append(std::to_string(entity));

		//Draw the selectable entity id
		if (ImGui::Selectable(text.c_str(), &entitySelection[entity]))
		{
			//Toggle all other entities off
			for (auto& e : entitySelection)
				e.second = false;
			selectedEntity = entity;
			entitySelection[entity] = true;

			EnableWindow(ImWindow::Inspector);
		}

		//Draw all children of this entity
		for (ecs::Entity child : ecs::GetComponent<une::Transform>(entity).children)
		{
			DrawHierarchyEntity(child, depth + 1);
		}
	}

	//Entity list and world hierarchy
	void DrawEntities()
	{
		ImGui::SetNextWindowSize(ImVec2(180, 300), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(1, 50), ImGuiCond_FirstUseEver);

		ImGui::Begin("Entities", &windowVisibility[ImWindow::Entities]);

		//Display all entities with a transform as a hierachy
		if (ImGui::CollapsingHeader("World Hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (ecs::Entity entity : une::transformSystem->entities)
			{
				une::Transform& tf = ecs::GetComponent<une::Transform>(entity);
				//Start from root entities
				if (!tf.parent)
					DrawHierarchyEntity(entity, 0);
			}
		}

		//Display all entities without a transform
		if (ImGui::CollapsingHeader("Non-transformable", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (ecs::Entity entity : ecs::usedEntities)
			{
				if (!ecs::HasComponent<une::Transform>(entity))
				{
					//Draw the selectable entity id
					if (ImGui::Selectable(std::to_string(entity).c_str(), &entitySelection[entity]))
					{
						//Toggle all other entities off
						for (auto& e : entitySelection)
							e.second = false;
						selectedEntity = entity;
						entitySelection[entity] = true;

						EnableWindow(ImWindow::Inspector);
					}
				}
			}
		}

		ImGui::End();
	}

	void DrawInspector()
	{
		ImGui::SetNextWindowSize(ImVec2(250,370), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(une::mainWindow->GetSize().x - 251, 50), ImGuiCond_FirstUseEver);

		ImGui::Begin("Entity Inspector", &windowVisibility[ImWindow::Inspector]);

		ImGui::Text("Selected Entity: %d", selectedEntity);
		ImGui::Separator();

		if (!ecs::EntityExists(selectedEntity))
		{
			ImGui::Text("Entity does not exist");
			ImGui::End();
			return;
		}

		//Print the tags attached to selected entity
		if (ImGui::CollapsingHeader("Tags"))
		{
			for (std::string tag : ecs::GetTags(selectedEntity))
			{
				ImGui::Text("%s", tag.c_str());
			}
		}

		//Print the readable names of all components attached to selected entity
		if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (int i = 0; i < ECS_MAX_COMPONENTS; i++)
			{
				if (ecs::entitySignatures[selectedEntity][i])
				{
					const char* componentName = ecs::componentIDToReadableName[i].c_str();
					if (ImGui::CollapsingHeader(componentName))
					{
						//If the component has a defined inspector, draw it here
						if (componentDrawFunctions.contains(componentName))
						{
							componentDrawFunctions.at(componentName)();
						}
						else
						{
							ImGui::Text("No component inspector implemented");
						}
					}
				}
			}
		}

		ImGui::End();
	}

	//Components

	//Transform component editor
	void DrawTransform()
	{
		if (ecs::HasComponent<une::Transform>(selectedEntity))
		{
			une::Transform& tf = ecs::GetComponent<une::Transform>(selectedEntity);

			//Position sliders
			float pos[3] = { (float)tf.position.x, (float)tf.position.y, (float)tf.position.z };
			if (ImGui::DragFloat3("Position", pos, 0.1f, 0.f, 0.f))
			{
				une::TransformSystem::SetPosition(selectedEntity, pos[0], pos[1], pos[2]);
			}

			//Rotation sliders
			float rot[3] = { (float)tf.rotation.x, (float)tf.rotation.y, (float)tf.rotation.z };
			if (ImGui::DragFloat3("Rotation", rot, 0.2f, 0.f, 0.f))
			{
				une::TransformSystem::SetRotation(selectedEntity, rot[0], rot[1], rot[2]);
			}

			//Scale sliders
			float scale[3] = { (float)tf.scale.x, (float)tf.scale.y, (float)tf.scale.z };
			if (ImGui::DragFloat3("Scale", scale, 0.02f, 0.f, 0.f))
			{
				une::TransformSystem::SetScale(selectedEntity, scale[0], scale[1], scale[2]);
			}

			ImGui::Separator();

			//Pivot sliders
			float pivot[3] = { (float)tf.pivot.x, (float)tf.pivot.y, (float)tf.pivot.z };
			if (ImGui::DragFloat3("Pivot", pivot, 0.2f, 0.f, 0.f))
			{
				une::TransformSystem::SetPivot(selectedEntity, pivot[0], pivot[1], pivot[2]);
			}

			//Rotation order selector
			ImGui::SetNextItemWidth(50);
			int currentOrder = tf.rotationOrder;
			if (ImGui::Combo("Rotation Order", &currentOrder, une::rotationOrderStrings, IM_ARRAYSIZE(une::rotationOrderStrings)))
			{
				tf.rotationOrder = (une::RotationOrder)currentOrder;
				tf.staleCache = true;
			}

			ImGui::Separator();

			//Parent
			ImGui::SetNextItemWidth(50);
			int parent = tf.parent;
			ImGui::InputInt("Parent", &parent, 0);
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				if (parent == 0)
					une::TransformSystem::RemoveParent(selectedEntity);
				else
				{
					if (ecs::HasComponent<une::Transform>(parent))
					{
						une::TransformSystem::AddParent(selectedEntity, parent);
					}
				}
			}

			//Children dropdown
			if (ImGui::TreeNode("Children"))
			{
				for (ecs::Entity child : tf.children)
				{
					//Draw the selectable entity id
					if (ImGui::Selectable(std::to_string(child).c_str(), &entitySelection[child]))
					{
						//Toggle all other entities off
						for (auto& e : entitySelection)
							e.second = false;
						selectedEntity = child;
						entitySelection[child] = true;
					}
				}
				ImGui::TreePop();
			}

			ImGui::Separator();

			//Copy Transform component string to clipboard
			if (ImGui::Button("Copy component to clipboard"))
			{
				std::string rotationOrder = "une::" + std::string(une::rotationOrderStrings[tf.rotationOrder]);

				//Format the component string
				char buff[1000];
				std::snprintf(buff, sizeof(buff),
					"une::Transform{%s, %s, %s, %s, %s}",
					tf.position.ToString().c_str(), tf.rotation.ToString().c_str(), tf.scale.ToString().c_str(),
					tf.pivot.ToString().c_str(), rotationOrder.c_str());

				ImGui::SetClipboardText(buff);
			}
		}
	}
}
