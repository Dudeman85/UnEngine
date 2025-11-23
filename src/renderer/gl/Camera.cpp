#include "renderer/gl/Camera.h"

#include <algorithm>

#include "glm/gtc/matrix_transform.hpp"

#include "renderer/gl/Window.h"
#include "renderer/UnifiedRenderer.h"



namespace une
{
	//Renders all camera views to the canvas
	void CameraSystem::Update()
	{
		//Sort the cameras by draw order
		std::vector<ecs::Entity> sortedEntities;
		for (ecs::Entity entity : entities)
			sortedEntities.push_back(entity);
		std::sort(sortedEntities.begin(), sortedEntities.end(),
			[](const ecs::Entity& lhs, const ecs::Entity& rhs)
			{
				return ecs::GetComponent<Camera>(lhs).drawOrder < ecs::GetComponent<Camera>(rhs).drawOrder;
			});

		for (ecs::Entity entity : sortedEntities)
		{
			Camera& cam = ecs::GetComponent<Camera>(entity);
			Transform& tf = ecs::GetComponent<Transform>(entity);

			if (!cam.enabled)
				continue;

			//Set the viewport pixel size
			const double viewportWidth = cam.viewport.x2 - cam.viewport.x1;
			const double viewportHeight = cam.viewport.y2 - cam.viewport.y1;
			const Vector2 windowSize = mainWindow->GetSize();
			glViewport(std::floor(cam.viewport.x1 * windowSize.x), std::floor(cam.viewport.y1 * windowSize.y),
				std::floor(viewportWidth * windowSize.x), std::floor(viewportHeight * windowSize.y));

			if (tf.staleCache)
				RecalculateView(entity);

			renderer::UnifiedRenderPass(entity);
		}
	}

	void CameraSystem::MakeOrtho(ecs::Entity e, float width, float height, float nearPlane, float farPlane)
	{
		Camera& cam = ecs::GetComponent<Camera>(e);

		cam.width = width;
		cam.height = height;
		cam.nearPlane = nearPlane;
		cam.farPlane = farPlane;

		RecalculateProjection(e);
	}

	void CameraSystem::MakePerspective(ecs::Entity e, float fov, float nearPlane, float farPlane)
	{
		Camera& cam = ecs::GetComponent<Camera>(e);

		cam.fov = fov;
		cam.nearPlane = nearPlane;
		cam.farPlane = farPlane;

		RecalculateProjection(e);
	}

	//Recalculates the view matrix, called automatically when transform changes
	void CameraSystem::RecalculateView(ecs::Entity e)
	{
		Camera& cam = ecs::GetComponent<Camera>(e);
		Transform& t = ecs::GetComponent<Transform>(e);

		cam.view = glm::mat4(1.0f);
		cam.view = glm::translate(cam.view, -t.position.ToGlm());
		cam.view = glm::rotate(cam.view, (float)glm::radians(t.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		cam.view = glm::rotate(cam.view, (float)glm::radians(t.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		cam.view = glm::rotate(cam.view, (float)glm::radians(t.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	//Recalcultes the projection matrix, call this if you manually change fov, perspectve, nearPlane, or farPlane
	void CameraSystem::RecalculateProjection(ecs::Entity e)
	{
		Camera& cam = ecs::GetComponent<Camera>(e);

		if (cam.perspective)
		{
			const double aspectRatio = (double)(cam.viewport.x2 - cam.viewport.x1) / (double)(cam.viewport.y2 - cam.viewport.y1);
			cam.projection = glm::perspective(glm::radians(cam.fov), (float)aspectRatio, cam.nearPlane, cam.farPlane);
		}
		else
		{
			cam.projection = glm::ortho(-cam.width / 2, cam.width / 2, -cam.height / 2, cam.height / 2, cam.nearPlane, cam.farPlane);
		}
	}
}
