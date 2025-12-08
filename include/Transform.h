#pragma once
#include <set>

#include "glm/glm.hpp"

#include "ECS.h"
#include "Vector.h"

namespace une
{
	enum RotationOrder : int { XYZ, XZY, YXZ, ZXY, YZX, ZYX };
	inline const char* rotationOrderStrings[] = { "XYZ", "XZY", "YXZ", "ZXY", "YZX", "ZYX" };

	//Transform component
	ECS_REGISTER_COMPONENT(Transform)
	struct Transform
	{
		//The local position, relative to parent
		Vector3 position;
		//The local rotation, relative to parent
		Vector3 rotation;
		//The local scale, relative to parent
		Vector3 scale = Vector3(1.0f);
		//The origin/pivot point offset
		Vector3 pivot;
		//What order should euler rotation be applied in
		RotationOrder rotationOrder = XYZ;

		//The parent entity of this entity, If set this transform will be relative to the parent
		ecs::Entity parent = 0;
		//All the children of this entity
		std::set<ecs::Entity> children;

		//If true updates all transform based caches, reverts to false next frame
		//WARNING: This will not update if transform is manually changed
		bool staleCache = true;
	};

	//Transform system, Requires Transform component
	ECS_REGISTER_SYSTEM(TransformSystem, Transform)
	class TransformSystem : public ecs::System
	{
	public:
		void Update();

		//Destructor for the Transform component
		static void OnTransformRemoved(ecs::Entity entity, Transform& transform);

		//Translate an entity by dx, dy, and dz
		static void Translate(ecs::Entity entity, float dx, float dy, float dz = 0);
		//Translate an entity by dt
		static void Translate(ecs::Entity entity, Vector3 dt);
		//Set the world position of entity
		static void SetPosition(ecs::Entity entity, float x, float y, float z = 0);
		//Set the world position of entity
		static void SetPosition(ecs::Entity entity, Vector3 position);
		//Rotate an entity by euler angles dx, dy, and dz
		static void Rotate(ecs::Entity entity, float dx, float dy, float dz = 0);
		//Rotate an entity by euler angles dr
		static void Rotate(ecs::Entity entity, Vector3 dr);
		//Set the euler rotation of an entity
		static void SetRotation(ecs::Entity entity, float x, float y, float z = 0);
		//Set the euler rotation of an entity
		static void SetRotation(ecs::Entity entity, Vector3 rotation);
		//Set the origin/pivot point of an entity
		static void SetPivot(ecs::Entity entity, float x, float y, float z = 0);
		//Set the origin/pivot point of an entity
		static void SetPivot(ecs::Entity entity, Vector3 pivot);
		//Scale an entity by dx, dy, and dz
		static void Scale(ecs::Entity entity, float dx, float dy, float dz = 0);
		//Scale an entity by ds
		static void Scale(ecs::Entity entity, Vector3 ds);
		//Set the scale of an entity
		static void SetScale(ecs::Entity entity, float x, float y, float z = 0);
		//Set the scale of an entity
		static void SetScale(ecs::Entity entity, Vector3 scale);

		//Returns true if entity does not have parent as child
		static bool RecursiveCheckChildren(ecs::Entity entity, ecs::Entity parent);
		//Add a parent entity to a child entity
		static void AddParent(ecs::Entity child, ecs::Entity parent);
		//Remove a parent entity from a child entity, This will place the child to root
		static void RemoveParent(ecs::Entity child);

		//Get the right (x) vector of a transform
		static Vector3 RightVector(ecs::Entity entity);
		//Get the up (y) vector of a transform
		static Vector3 UpVector(ecs::Entity entity);
		//Get the forward (z) vector of a transform
		static Vector3 ForwardVector(ecs::Entity entity);

		//Get the distance between two entities
		static double Distance(ecs::Entity a, ecs::Entity b);

		//Get the angle of b with a as the origin
		//TODO Fix this to work in 3D
		static float Angle(ecs::Entity a, ecs::Entity b);

		//Calculate the global transform of an entity, this is not a reference and does not affect the original transform
		static Transform GetGlobalTransform(ecs::Entity entity);

		static glm::mat4 GetLocalTranformMatrix(ecs::Entity entity);

		//Get the global Transform matrix of an entity after all parent transforms have been applied
		static glm::mat4 GetGlobalTransformMatrix(ecs::Entity entity);

		//Applies transforms to vertices and returns the transformed vertices
		static std::vector<Vector3> ApplyTransforms(const std::vector<Vector3>& vertices, const Transform& transform);

		//Applies relevant 2D transforms to given 2D vertices and returns the transformed vertices
		static std::vector<Vector2> ApplyTransforms2D(const std::vector<Vector2>& vertices, const Transform& transform);

		static void ApplyRotation(glm::mat4& mat, Vector3 eulers, RotationOrder order);
	};
}