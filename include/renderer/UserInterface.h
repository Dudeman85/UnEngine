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
		UICanvas(const Vector3& pos = 0, const Vector3& rot = 0, const Vector3& scale = 1, float near = -1000, float far = 1000);

		void SetScale(const Vector3& s);
		void SetPosition(const Vector3& pos);
		void SetRotation(const Vector3& rot);
		void SetNearFar(float near, float far);

		Vector3 GetScale() const;
		Vector3 GetPosition() const;
		Vector3 GetRotation() const;
		Vector2 GetNearFar() const;
		glm::mat4 GetProjection() const;

		static glm::mat4 GetTransformForEntity(ecs::Entity entity);

	private:
		void RecalculateProjection();

		float near, far;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
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