#include "renderer/PrimitiveRenderer.h"

#include <algorithm>
#include <ranges>
#include <glm/gtc/type_ptr.hpp>

#include "assimp/code/AssetLib/3MF/3MFXmlTags.h"

namespace une
{
	Primitive::Primitive(const std::vector<double>& vertices, const std::vector<unsigned int>& indices)
	{
		numVertices = indices.size();

		//Make the Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Bind the Vertex Array Object
		glBindVertexArray(VAO);

		//Bind the Vertex Bufer Object and set vertices
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		//Bind and set indices to EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

		//Configure Vertex attribute at location 0 aka position
		glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), nullptr);
		glEnableVertexAttribArray(0);

		//Unbind all buffers and arrays
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	Primitive::~Primitive()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	//Create a line starting at p1 and ending at p2
	Primitive Primitive::Line(Vector3 p1, Vector3 p2)
	{
		//Rectangle vertices start at top left and go clockwise to bottom left
		std::vector<double> vertices
		{
			//Positions
			p1.x, p1.y, p1.z,
			p2.x, p2.y, p2.z,
		};
		//Indices to draw a line
		std::vector<unsigned int> indices
		{
			0, 1
		};

		//Create the primitive object from vertice data
		return Primitive(vertices, indices);
	}

	//Create a triangle from three vertices, Defaults to equilateral triangle
	Primitive Primitive::Triangle(Vector3 v1, Vector3 v2, Vector3 v3)
	{
		//Rectangle vertices start at bottom left and go clockwise to bottom right
		std::vector<double> vertices
		{
			//Positions
			v1.x, v1.y, v1.z,
			v2.x, v2.y, v2.z,
			v3.x, v3.y, v3.z,
		};
		//Indices to draw a triangle
		std::vector<unsigned int> indices
		{
			0, 1, 2,
		};

		//Create the primitive object from vertice data
		return Primitive(vertices, indices);
	}

	//Create a rectangle from four vertices going clockwise, Defaults to square
	Primitive Primitive::Rectangle(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4)
	{
		//Rectangle vertices start at bottom left and go clockwise to bottom right
		std::vector<double> vertices
		{
			//Positions
			v1.x, v1.y, v1.z,
			v2.x, v2.y, v2.z,
			v3.x, v3.y, v3.z,
			v4.x, v4.y, v4.z,
		};
		//Indices to draw a rectangle
		std::vector<unsigned int> indices
		{
			0, 1, 2,
			2, 3, 0,
		};

		//Create the primitive object from vertice data
		return Primitive(vertices, indices);
	}

	//Create a polygon from provided 3D vertices, going clockwise
	Primitive Primitive::Polygon(const std::vector<Vector3>& verts)
	{
		//Move all Vector3 vertices to a simple float vector
		std::vector<double> vertices;
		//Automaticaly create indices to draw triangles
		std::vector<unsigned int> indices;
		for (int i = 0; i < verts.size(); i++)
		{
			vertices.push_back(verts[i].x);
			vertices.push_back(verts[i].y);
			vertices.push_back(verts[i].z);
			indices.push_back(i);
		}

		//Create the primitive object from vertice data
		return Primitive(vertices, indices);
	}
	//Create a polygon from provided 2D vertices, going clockwise
	Primitive Primitive::Polygon(const std::vector<Vector2>& verts)
	{
		//Move all Vector2 vertices to a simple float vector
		std::vector<double> vertices;
		//Automaticaly create indices to draw triangles
		std::vector<unsigned int> indices;
		for (int i = 0; i < verts.size(); i++)
		{
			vertices.push_back(verts[i].x);
			vertices.push_back(verts[i].y);
			vertices.push_back(0);
			indices.push_back(i);
		}

		//Create the primitive object from vertice data
		return Primitive(vertices, indices);
	}

	namespace renderer
	{
		//Initialize the default shader
		void PrimitiveRenderSystem::Init()
		{
			//The default 3D model shader
			shader = new Shader(
				R"(
			#version 330 core
			layout(location = 0) in vec3 aPos;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;
			uniform vec4 color;

			out vec4 vertexColor;

			void main()
			{
				vertexColor = color;
				gl_Position = projection * view * model * vec4(aPos, 1.0);
			}
			)",
				R"(
			#version 330 core
			out vec4 FragColor;

			in vec4 vertexColor;

			void main()
			{
				if(vertexColor.a <= 0.02)
					discard;
				FragColor = vertexColor;
			}
			)", false);
		}

		//Sorts the primitives into their draw layers
		void PrimitiveRenderSystem::Prepass()
		{
			opaqueWorldEntities.clear();
			transparentWorldEntities.clear();
			opaqueUIEntities.clear();
			transparentUIEntities.clear();

			//Sort all entities into their draw orders
			for (ecs::Entity entity : entities)
			{
				PrimitiveRenderer& primitiveRenderer = ecs::GetComponent<PrimitiveRenderer>(entity);
				Vector3 pos = TransformSystem::GetGlobalTransform(entity).position;
				Color srgb = primitiveRenderer.color.AsSRGB();

				if (!primitiveRenderer.enabled)
					continue;
				if (primitiveRenderer.uiElement)
				{
					if (srgb.a > 0.02 && srgb.a < 0.98)
						transparentUIEntities.push_back({entity, pos, DrawRenderable});
					else
						opaqueUIEntities.push_back(entity);
				}
				else
				{
					if (srgb.a > 0.02 && srgb.a < 0.98)
						transparentWorldEntities.push_back({entity, pos, DrawRenderable});
					else
						opaqueWorldEntities.push_back(entity);
				}
			}
		}

		//Draws all entities in the opaqueWorldEntities list
		void PrimitiveRenderSystem::DrawOpaqueWorldEntities(ecs::Entity cameraEntity)
		{
			for (ecs::Entity entity : opaqueWorldEntities)
			{
				DrawEntity(entity, cameraEntity);
			}
		}

		//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
		void PrimitiveRenderSystem::DrawOpaqueUIEntities(ecs::Entity cameraEntity)
		{
			for (ecs::Entity entity : opaqueUIEntities)
			{
				DrawEntity(entity, cameraEntity);
			}
		}

		//Static version of DrawEntity for renderer
		void PrimitiveRenderSystem::DrawRenderable(const Renderable& r, ecs::Entity cameraEntity)
		{
			ecs::GetSystem<PrimitiveRenderSystem>()->DrawEntity(r.entity, cameraEntity);
		}

		//Draw a primitive to the screen
		void PrimitiveRenderSystem::DrawEntity(ecs::Entity entity, ecs::Entity cameraEntity)
		{
			Transform transform = TransformSystem::GetGlobalTransform(entity);
			PrimitiveRenderer& primitiveRenderer = ecs::GetComponent<PrimitiveRenderer>(entity);

			DrawPrimitive(primitiveRenderer.primitive, cameraEntity, primitiveRenderer.color, primitiveRenderer.uiElement ? ui : normal,
				transform.position, transform.rotation, transform.scale);
		}

		//Draw a primitive to the screen, does not require an entity
		void PrimitiveRenderSystem::DrawPrimitive(const Primitive* primitive, ecs::Entity cameraEntity, const Color& color, DrawPriority prio,
			Vector3 position, Vector3 rotation, Vector3 scale)
		{
			Camera& cam = ecs::GetComponent<Camera>(cameraEntity);

			shader->Use();

			glBindVertexArray(primitive->VAO);

			//Create the model matrix
			glm::mat4 model = glm::mat4(1.0);
			//Position
			model = glm::translate(model, position.ToGlm());
			//X, Y, Z euler rotations
			model = glm::rotate(model, (float)glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, (float)glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			//Scale
			model = glm::scale(model, scale.ToGlm());

			//Give the shader the model matrix
			int modelLoc = glGetUniformLocation(shader->ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			//Give the shader the primitive's color
			int colorLoc = glGetUniformLocation(shader->ID, "color");
			const Color srgb = color.AsSRGB();
			glUniform4f(colorLoc, srgb.r, srgb.g, srgb.b, srgb.a);

			//Get the view and projection locations
			int viewLoc = glGetUniformLocation(shader->ID, "view");
			int projLoc = glGetUniformLocation(shader->ID, "projection");

			if (prio == DrawPriority::ui)
			{
				//Render UI elements independent of camera's view and projection
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
			}
			else
			{
				if (prio == DrawPriority::aboveAll)
					glDisable(GL_DEPTH_BUFFER_BIT);
				//Render World entities based on camera's view and projection
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam.view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam.projection));
			}

			//TODO: Fix filled draw
			glDrawElements(GL_LINE_LOOP, primitive->numVertices, GL_UNSIGNED_INT, 0);

			glEnable(GL_DEPTH_BUFFER_BIT);
			glBindVertexArray(0);
		}

		const std::vector<Renderable>& PrimitiveRenderSystem::GetTransparentWorldEntities()
		{
			return transparentWorldEntities;
		}
		const std::vector<Renderable>& PrimitiveRenderSystem::GetTransparentUIEntities()
		{
			return transparentUIEntities;
		}

		Shader* PrimitiveRenderSystem::shader = nullptr;
	}
}
