#include "renderer/UserInterface.h"

#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"
#include "renderer/gl/Window.h"

namespace une
{
	UICanvas::UICanvas(const Vector3& pos, const Vector3& rot, const Vector3& scale, float near, float far)
	{
		this->position = pos;
		this->rotation = rot;
		this->scale = scale;
		this->near = near;
		this->far = far;
		RecalculateProjection();
	}

	void UICanvas::SetScale(const Vector3& s)
	{
		this->scale = s;
		RecalculateProjection();
	}
	void UICanvas::SetNearFar(float near, float far)
	{
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

	Vector3 UICanvas::GetScale() const
	{
		return scale;
	}
	Vector3 UICanvas::GetPosition() const
	{
		return position;
	}
	Vector3 UICanvas::GetRotation() const
	{
		return rotation;
	}
	Vector2 UICanvas::GetNearFar() const
	{
		return {near, far};
	}
	glm::mat4 UICanvas::GetProjection() const
	{
		return projection;
	}

	glm::mat4 UICanvas::GetTransformForEntity(ecs::Entity entity)
	{
		const UIElement& ui = ecs::GetComponent<UIElement>(entity);

		glm::mat4 transformMatrix = glm::mat4(1.0);
		transformMatrix = glm::translate(transformMatrix, ui.canvas->GetPosition().ToGlm());
		TransformSystem::ApplyRotation(transformMatrix, ui.canvas->GetRotation(), XYZ);
		Vector3 anchorPos = ui.anchor * (ui.canvas->GetScale() * mainWindow->GetSize() / 2);
		transformMatrix = glm::translate(transformMatrix, anchorPos.ToGlm());

		return transformMatrix;
	}

	void UICanvas::RecalculateProjection()
	{
		const float width = scale.x * mainWindow->GetSize().x;
		const float height = scale.y * mainWindow->GetSize().y;
		projection = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, near, far);
	}
}
