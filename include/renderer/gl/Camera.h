#pragma once

#include <glm/glm.hpp>

#include "Vector.h"

namespace engine
{
	///Class for creating and storing data for the camera
	class Camera
	{
	public:
		///Constructor
		Camera(float w, float h, glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f), glm::vec3 rot = glm::vec3(0.f, 0.f, 0.f));

		///Sets the camera position in world space
		void SetPosition(Vector3 pos);

		///Move the camera in world space. Camera's origin is in its center
		void Translate(Vector3 dPos = 0.f);

		///Set the rotation of the camera the camera in world space
		void SetRotation(Vector3 rot);

		///Rotate the camera in world space
		void Rotate(Vector3 dRot);

		///Sets the width and height of the camera
		void SetDimensions(float w, float h);

		///Get the projection matrix for this camera. is perspective is set to true, use perspective projection instead of orthographic
		glm::mat4 GetProjectionMatrix();

		///Get the model matrix for this camera, aka the view matrix
		glm::mat4 GetViewMatrix();

		///Position of the camera
		glm::vec3 position;
		///Rotation of the camera
		glm::vec3 rotation;
		///Bool to turn on and of different perspective modes
		bool perspective = false;
		///Width of the camera
		float width;
		///Height of the camera
		float height;
		///Farplane is a location in the final fantasy mmo i guess but also a datapoint for our 3rd perspective camera
		float farPlane = 10000;
		///Field of view of the camera
		float fov = 45;
	};
}