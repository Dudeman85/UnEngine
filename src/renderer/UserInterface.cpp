#include "renderer/UserInterface.h"

#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"

namespace une
{
	UICanvas::UICanvas(float width, float height, const Vector3& pos, const Vector3& rot, float near, float far)
	{
		this->width = width;
		this->height = height;
		this->position = pos;
		this->rotation = rot;
		this->near = near;
		this->far = far;
		RecalculateProjection();
	}

	void UICanvas::SetSize(float width, float height)
	{
		this->width = width;
		this->height = height;
		RecalculateProjection();
	}
	void UICanvas::SetSize(float width, float height, float near, float far)
	{
		this->width = width;
		this->height = height;
		this->near = near;
		this->far = far;
		RecalculateProjection();
	}
	void UICanvas::SetPosition(const Vector3& pos)
	{
		this->position = pos;
	}
	void UICanvas::SetRotation(const Vector3& rot)
	{
		this->rotation = rot;
	}

	//Returns size where x = width, y = height, z = near, w = far
	Vector4 UICanvas::GetSize() const
	{
		return Vector4(width, height, near, far);
	}
	Vector3 UICanvas::GetPosition() const
	{
		return position;
	}
	Vector3 UICanvas::GetRotation() const
	{
		return rotation;
	}
	glm::mat4 UICanvas::GetTransformForEntity(ecs::Entity entity)
	{
		UIElement& ui = ecs::GetComponent<UIElement>(entity);
		Transform& transform = ecs::GetComponent<Transform>(entity);


	}
	glm::mat4 UICanvas::GetProjection() const
	{
		return projection;
	}

	void UICanvas::RecalculateView()
	{
		view = glm::mat4(1.0f);
		view = glm::translate(view, position.ToGlm());
		view = glm::rotate(view, (float)glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, (float)glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, (float)glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	void UICanvas::RecalculateProjection()
	{
		projection = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, near, far);
	}
}
