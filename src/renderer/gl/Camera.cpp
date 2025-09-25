#include "renderer/gl/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace une
{
	Camera::Camera(float w, float h, glm::vec3 pos, glm::vec3 rot)
	{
		width = w;
		height = h;
		position = pos;
		rotation = rot;
	}

	///Sets the camera position in world space
	void Camera::SetPosition(Vector3 pos)
	{
		//I really have no idea why these have to be divided by 2, but otherwise the coordinates don't match the sprites
		position.x = pos.x / 2;
		position.y = pos.y / 2;
		position.z = pos.z / 2;
	}

	///Move the camera in world space. Camera's origin is in its center
	void Camera::Translate(Vector3 dPos)
	{
		//I really have no idea why these have to be divided by 2, but otherwise the coordinates don't match the sprites
		position.x += dPos.x / 2;
		position.y += dPos.y / 2;
		position.z += dPos.z / 2;
	}

	///Set the rotation of the camera the camera in world space
	void Camera::SetRotation(Vector3 rot)
	{
		rotation.x = rot.x;
		rotation.y = rot.y;
		rotation.z = rot.z;
	}

	///Rotate the camera in world space
	void Camera::Rotate(Vector3 dRot)
	{
		rotation.x += dRot.x;
		rotation.y += dRot.y;
		rotation.z += dRot.z;
	}

	///Sets the width and height of the camera
	void Camera::SetDimensions(float w, float h)
	{
		width = w;
		height = h;
	}

	///Get the projection matrix for this camera. is perspective is set to true, use perspective projection instead of orthographic
	glm::mat4 Camera::GetProjectionMatrix()
	{
		if (perspective)
			return glm::perspective(glm::radians(fov), width / height, 0.01f, farPlane);
		else
			return glm::ortho(position.x - width / 2, position.x + width / 2, position.y - height / 2, position.y + height / 2, 0.01f, farPlane);
	}

	///Get the model matrix for this camera, aka the view matrix
	glm::mat4 Camera::GetViewMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);
		//Position
		model = glm::translate(model, -position);
		//X, Y, Z euler rotations
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		return model;
	}
}