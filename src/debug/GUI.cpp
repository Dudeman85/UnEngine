#include "debug/GUI.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "ECS.h"
#include "UnEngine.h"
#include "debug/Primitives.h"
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
		//Add tags staring with # to hierarchy
		std::vector<std::string> tags = ecs::GetTags(entity);
		char spacer = ':';
		for (std::string& tag : tags)
		{
			if (tag[0] == '#')
			{
				tag[0] = ' ';
				text.append(spacer + tag);
				spacer = ',';
			}
		}

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
			for (uint16_t i = 0; i < ECS_MAX_COMPONENTS; i++)
			{
				if (ecs::GetSignature(selectedEntity)[i])
				{
					const std::string componentName = ecs::GetComponentNameByID(i);
					if (ImGui::CollapsingHeader(componentName.c_str()))
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

	void TransformInspector()
	{
		une::Transform& tf = ecs::GetComponent<une::Transform>(selectedEntity);

		//Visualize transform origin
		une::Transform globalTf = une::TransformSystem::GetGlobalTransform(selectedEntity);
		globalTf.scale = 15;
		globalTf.pivot = 0;
		std::vector<une::Vector3> verts{{0, 0, 0}, {0, 1, 0}, {1, 0, 0}};
		verts = une::TransformSystem::ApplyTransforms(verts, globalTf);
		debug::DrawTriangle(verts[0], verts[1], verts[2], une::Color(17, 156, 45), false);

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
		if (ImGui::Button("Copy Transform to clipboard"))
		{
			std::string rotationOrder = "une::" + std::string(une::rotationOrderStrings[tf.rotationOrder]);

			//Format the component string
			char buff[1000];
			std::snprintf(buff, sizeof(buff),
				"une::Transform{.position = %s, .rotation = %s, .scale = %s, .pivot = %s, .rotationOrder = %s}",
				tf.position.ToString().c_str(), tf.rotation.ToString().c_str(), tf.scale.ToString().c_str(),
				tf.pivot.ToString().c_str(), rotationOrder.c_str());

			ImGui::SetClipboardText(buff);
		}

		ImGui::Separator();
	}

	void PolygonColliderInspector()
	{
		une::PolygonCollider& collider = ecs::GetComponent<une::PolygonCollider>(selectedEntity);

		//Vertices
		if (ImGui::TreeNode("Vertices"))
		{
			//Size the vector
			int size = collider.vertices.size();
			ImGui::SetNextItemWidth(100);
			if (ImGui::InputInt("Amount", &size))
			{
				if ((ImGui::IsItemDeactivated() || ImGui::IsItemClicked()) && size >= 0)
				{
					collider.vertices.resize(size);
					if (ecs::HasComponent<une::Transform>(selectedEntity))
						ecs::GetComponent<une::Transform>(selectedEntity).staleCache = true;
				}
			}

			//Display every Vertex
			for (int i = 0; i < collider.vertices.size(); i++)
			{
				float vert[2] = {(float)collider.vertices[i].x, (float)collider.vertices[i].y};
				if (ImGui::DragFloat2(std::to_string(i).c_str(), vert, 0.1f, 0.f, 0.f))
				{
					collider.vertices[i] = {vert[0], vert[1]};
					if (ecs::HasComponent<une::Transform>(selectedEntity))
						ecs::GetComponent<une::Transform>(selectedEntity).staleCache = true;
				}
			}
			ImGui::TreePop();
		}

		//Simple members
		ImGui::Separator();
		ImGui::Checkbox("Trigger", &collider.trigger);
		ImGui::Checkbox("Visualise", &collider.visualise);
		ImGui::SetNextItemWidth(100);
		ImGui::InputInt("Layer", &collider.layer);
		ImGui::SetNextItemWidth(100);
		if (ImGui::DragFloat("Rotation Override", &collider.rotationOverride, 0.2f, -1.f, 360.f, "%.1f"))
		{
			if (ecs::HasComponent<une::Transform>(selectedEntity))
				ecs::GetComponent<une::Transform>(selectedEntity).staleCache = true;
		}

		//AABB
		ImGui::Separator();
		if (ImGui::TreeNode("Bounding Box"))
		{
			ImGui::Text("Top:    %.1f", collider.bounds[0]);
			ImGui::Text("Right:  %.1f", collider.bounds[1]);
			ImGui::Text("Bottom: %.1f", collider.bounds[2]);
			ImGui::Text("Left:   %.1f", collider.bounds[3]);
			ImGui::TreePop();
		}

		ImGui::Separator();

		//Copy PolygonCollider component string to clipboard
		if (ImGui::Button("Copy PolygonCollider to clipboard"))
		{
			std::string verts = "{";
			for (const une::Vector2 vert : collider.vertices)
			{
				verts += vert.ToString() + ", ";
			}
			verts += "}";

			//Format the component string
			char buff[10000];
			std::snprintf(buff, sizeof(buff),
				"une::PolygonCollider{.vertices = %s, .trigger = %d, .layer = %d, .rotationOverride = %.1f, .visualise = %d}",
				verts.c_str(), collider.trigger, collider.layer,collider.rotationOverride, collider.visualise);

			ImGui::SetClipboardText(buff);
		}

		ImGui::Separator();
	}

	void RigidbodyInspector()
	{
		une::Rigidbody& rb = ecs::GetComponent<une::Rigidbody>(selectedEntity);

		//Velocity sliders
		float v[3] = { (float)rb.velocity.x, (float)rb.velocity.y, (float)rb.velocity.z };
		if (ImGui::DragFloat3("Velocity", v, 1, 0, 0, "%.1f"))
		{
			rb.velocity = {v[0], v[1], v[2]};
		}

		//Simple members
		ImGui::DragFloat("Mass", &rb.mass, 1, 0, 0, "%.1f");
		ImGui::DragFloat("Gravity Scale", &rb.gravityScale, 0.1, 0, 0, "%.1f");
		ImGui::DragFloat("Drag", &rb.drag, 0.01, 0, 999999, "%.3f");
		ImGui::DragFloat("Restitution", &rb.restitution, 0.01, 0, 1, "%.3f");
		ImGui::Checkbox("Kinematic", &rb.kinematic);

		ImGui::Separator();

		//Copy Rigidbody component string to clipboard
		if (ImGui::Button("Copy Rigidbody to clipboard"))
		{
			//Format the component string
			char buff[10000];
			std::snprintf(buff, sizeof(buff),
				"une::Rigidbody{.velocity = %s, .mass = %.1f, .gravityScale = %.1f, .drag = %.3f, .restitution = %.3f, .kinematic = %d}",
				rb.velocity.ToString().c_str(), rb.mass, rb.gravityScale, rb.drag, rb.restitution, rb.kinematic);

			ImGui::SetClipboardText(buff);
		}

		ImGui::Separator();
	}

	void TextRendererInspector()
	{
		une::TextRenderer& tr = ecs::GetComponent<une::TextRenderer>(selectedEntity);

		ImGui::Text("Font: %s", tr.font->name.c_str());
		ImGui::Separator();
		ImGui::InputTextMultiline("Text", &tr.text, ImVec2(0, ImGui::GetTextLineHeight() * 4.5));
		ImGui::DragInt("Size", &tr.size, 1, 1, 256);

		//Color picker
		une::Color c = tr.color.AsSRGB();
		float col[4] = {c.r, c.g, c.b, c.a};
		if (ImGui::ColorEdit4("Color", col, ImGuiColorEditFlags_NoInputs))
		{
			tr.color = une::Color(col[0], col[1], col[2], col[3]);
		}
		ImGui::Checkbox("Enabled", &tr.enabled);

		ImGui::Separator();

		//Copy TextRenderer component string to clipboard
		if (ImGui::Button("Copy TextRenderer to clipboard"))
		{
			//Format the component string
			char buff[10000];
			std::snprintf(buff, sizeof(buff),
				"une::TextRenderer{.font = &font, .text = \"%s\", .size = %d, .color = une::%s, .enabled = %d }",
				tr.text.c_str(), tr.size, tr.color.ToString().c_str(), tr.enabled);

			ImGui::SetClipboardText(buff);
		}

		ImGui::Separator();
	}

	void CameraInspector()
	{
		une::Camera& c = ecs::GetComponent<une::Camera>(selectedEntity);

		if (ImGui::Checkbox("Perspective", &c.perspective))
		{
			une::CameraSystem::RecalculateProjection(selectedEntity);
		}

		//Ortho width and height
		float wh[2] = {c.width, c.height};
		if (ImGui::DragFloat2("Width/Height", wh, 1, 1, 999999, "%.1f"))
		{
			c.width = wh[0];
			c.height = wh[1];
			une::CameraSystem::RecalculateProjection(selectedEntity);
		}

		//Perspective field of view
		if (ImGui::DragFloat("FOV", &c.fov, 1, 1, 179, "%.1f"))
		{
			une::CameraSystem::RecalculateProjection(selectedEntity);
		}

		//Near and far clip planes
		float nf[2] = {c.nearPlane, c.farPlane};
		if (ImGui::DragFloat2("Near/Far", wh, 1, -999999, 999999, "%.1f"))
		{
			c.nearPlane = nf[0];
			c.farPlane = nf[1];
			une::CameraSystem::RecalculateProjection(selectedEntity);
		}

		ImGui::InputInt("Draw Order", &c.drawOrder);

		ImGui::Separator();
		//Viewport ndc sliders
		if (ImGui::TreeNode("Viewport"))
		{
			float bl[2] = {(float)c.viewport.x1, (float)c.viewport.y1};
			ImGui::DragFloat2("Bottom-left", bl, 0.01, 0, 1, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			float tr[2] = {(float)c.viewport.x2, (float)c.viewport.y2};
			ImGui::DragFloat2("Top-Right", tr, 0.01, 0, 1, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			c.viewport = {bl[0], bl[1], tr[0], tr[1]};

			ImGui::TreePop();
		}

		ImGui::Separator();
		//Read-only View and Projection matrices
		if (ImGui::TreeNode("Internal"))
		{
			//View
			ImGui::Text("View Matrix");
			ImGui::BeginTable("View Matrix", 4, ImGuiTableFlags_Borders);
			for (int row = 0; row < 4; row++)
			{
				ImGui::TableNextRow();
				for (int col = 0; col < 4; col++)
				{
					ImGui::TableSetColumnIndex(col);
					ImGui::Text("%.3f", c.view[col][row]);
				}
			}
			ImGui::EndTable();

			//Projection
			ImGui::Text("Projection Matrix");
			ImGui::BeginTable("Projection Matrix", 4, ImGuiTableFlags_Borders);
			for (int row = 0; row < 4; row++)
			{
				ImGui::TableNextRow();
				for (int col = 0; col < 4; col++)
				{
					ImGui::TableSetColumnIndex(col);
					ImGui::Text("%.3f", c.projection[col][row]);
				}
			}
			ImGui::EndTable();

			ImGui::TreePop();
		}

		ImGui::Separator();
	}
}
