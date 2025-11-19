#pragma once

#include "ECS.h"
#include "Vector.h"
#include "glm/mat4x4.hpp"

//Fix win32 bs
#undef near
#undef far

namespace une
{
	class UICanvas
	{
	public:
		UICanvas(float width, float height, const Vector3& pos = 0, const Vector3& rot = 0, float near = -1000, float far = 1000);

		void SetSize(float width, float height);
		void SetSize(float width, float height, float near, float far);
		void SetPosition(const Vector3& pos);
		void SetRotation(const Vector3& rot);

		//Returns size where x = width, y = height, z = near, w = far
		Vector4 GetSize() const;
		Vector3 GetPosition() const;
		Vector3 GetRotation() const;
		glm::mat4 GetProjection() const;
		static glm::mat4 GetTransformForEntity(ecs::Entity entity);

	private:
		void RecalculateView();
		void RecalculateProjection();

		float width, height, near, far;
		Vector3 position;
		Vector3 rotation;
		glm::mat4 view;
		glm::mat4 projection;
	};

	ECS_REGISTER_COMPONENT(UIElement)
	struct UIElement
	{
		UICanvas* canvas = nullptr;
		//A point inside the canvas in ndc. Transform will be relative to it.
		Vector2 anchor;
	};
}