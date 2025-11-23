#include "Transform.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace une
{
	//Call this every frame
	void TransformSystem::Update()
	{
		for (ecs::Entity entity: entities)
		{
			Transform& transform = ecs::GetComponent<Transform>(entity);
			transform.staleCache = false;
		}
	}

	//Destructor for the Transform component
	void TransformSystem::OnTransformRemoved(ecs::Entity entity, Transform& transform)
	{
		//Delete all children
		for (ecs::Entity child: transform.children)
		{
			if (ecs::EntityExists(child))
				ecs::DestroyEntity(child);
		}
		//Remove self from parent's children
		if (ecs::EntityExists(transform.parent))
		{
			Transform& parentTransform = ecs::GetComponent<Transform>(transform.parent);
			parentTransform.children.erase(entity);
		}
	}

	//Translate an entity by dx, dy, and dz
	void TransformSystem::Translate(ecs::Entity entity, float dx, float dy, float dz)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.position.x += dx;
		transform.position.y += dy;
		transform.position.z += dz;

		transform.staleCache = true;
	}
	//Translate an entity by dt
	void TransformSystem::Translate(ecs::Entity entity, Vector3 dt)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.position += dt;

		transform.staleCache = true;
	}
	//Set the world position of entity
	void TransformSystem::SetPosition(ecs::Entity entity, float x, float y, float z)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.position.x = x;
		transform.position.y = y;
		transform.position.z = z;

		transform.staleCache = true;
	}
	//Set the world position of entity
	void TransformSystem::SetPosition(ecs::Entity entity, Vector3 position)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.position = position;

		transform.staleCache = true;
	}
	//Rotate an entity by euler angles dx, dy, and dz
	void TransformSystem::Rotate(ecs::Entity entity, float dx, float dy, float dz)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.rotation.x += dx;
		transform.rotation.y += dy;
		transform.rotation.z += dz;

		transform.staleCache = true;
	}
	//Rotate an entity by euler angles dr
	void TransformSystem::Rotate(ecs::Entity entity, Vector3 dr)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.rotation += dr;

		transform.staleCache = true;
	}
	//Set the euler rotation of an entity
	void TransformSystem::SetRotation(ecs::Entity entity, float x, float y, float z)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.rotation.x = x;
		transform.rotation.y = y;
		transform.rotation.z = z;

		transform.staleCache = true;
	}
	//Set the euler rotation of an entity
	void TransformSystem::SetRotation(ecs::Entity entity, Vector3 rotation)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.rotation = rotation;

		transform.staleCache = true;
	}
	//Set the origin/pivot point of an entity
	void TransformSystem::SetPivot(ecs::Entity entity, float x, float y, float z)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.pivot = {x, y, z};

		transform.staleCache = true;
	}
	//Set the origin/pivot point of an entity
	void TransformSystem::SetPivot(ecs::Entity entity, Vector3 pivot)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.pivot = pivot;

		transform.staleCache = true;
	}
	//Scale an entity by dx, dy, and dz
	void TransformSystem::Scale(ecs::Entity entity, float dx, float dy, float dz)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.scale.x += dx;
		transform.scale.y += dy;
		transform.scale.z += dz;

		transform.staleCache = true;
	}
	//Scale an entity by ds
	void TransformSystem::Scale(ecs::Entity entity, Vector3 ds)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.scale += ds;

		transform.staleCache = true;
	}
	//Set the scale of an entity
	void TransformSystem::SetScale(ecs::Entity entity, float x, float y, float z)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.scale.x = x;
		transform.scale.y = y;
		transform.scale.z = z;

		transform.staleCache = true;
	}
	//Set the scale of an entity
	void TransformSystem::SetScale(ecs::Entity entity, Vector3 scale)
	{
		Transform& transform = ecs::GetComponent<Transform>(entity);
		transform.scale = scale;

		transform.staleCache = true;
	}

	//Add a parent entity to a child entity
	void TransformSystem::AddParent(ecs::Entity child, ecs::Entity parent)
	{
		Transform& parentTransform = ecs::GetComponent<Transform>(parent);
		Transform& childTransform = ecs::GetComponent<Transform>(child);

		parentTransform.children.insert(child);
		childTransform.parent = parent;
	}
	//Remove a parent entity from a child entity, This will place the child to root
	void TransformSystem::RemoveParent(ecs::Entity child, ecs::Entity parent)
	{
		Transform& parentTransform = ecs::GetComponent<Transform>(parent);
		Transform& childTransform = ecs::GetComponent<Transform>(child);

		parentTransform.children.erase(child);
		childTransform.parent = 0;
	}

	//Get the right (x) vector of a transform
	Vector3 TransformSystem::RightVector(ecs::Entity entity)
	{
		const Transform& transform = ecs::GetComponent<Transform>(entity);

		//Create the rotation matrix
		glm::mat4 rotation = glm::mat4(1.0f);
		//Apply euler rotations in reverse desired order
		ApplyRotation(rotation, Vector3() - transform.rotation, (RotationOrder) (ZYX - transform.rotationOrder));

		//Right vector is x
		glm::vec4 right(1, 0, 0, 0);

		right = right * rotation;

		return Vector3(right.x, right.y, right.z);
	}
	//Get the up (y) vector of a transform
	Vector3 TransformSystem::UpVector(ecs::Entity entity)
	{
		const Transform& transform = ecs::GetComponent<Transform>(entity);

		//Create the rotation matrix
		glm::mat4 rotation = glm::mat4(1.0f);
		//Apply euler rotations in reverse desired order
		ApplyRotation(rotation, Vector3() - transform.rotation, (RotationOrder) (ZYX - transform.rotationOrder));

		//Up vector is y
		glm::vec4 up(0, 1, 0, 0);

		up = up * rotation;

		return Vector3(up.x, up.y, up.z);
	}
	//Get the forward (z) vector of a transform
	Vector3 TransformSystem::ForwardVector(ecs::Entity entity)
	{
		const Transform& transform = ecs::GetComponent<Transform>(entity);

		//Create the rotation matrix
		glm::mat4 rotation = glm::mat4(1.0f);
		//Apply euler rotations in reverse desired order
		ApplyRotation(rotation, Vector3(0) - transform.rotation, (RotationOrder)(ZYX - transform.rotationOrder));

		//Forward vector is z
		glm::vec4 forward(0, 0, 1, 0);

		forward = forward * rotation;

		return Vector3(forward.x, forward.y, forward.z);
	}

	//Get the distance between two entities
	double TransformSystem::Distance(ecs::Entity a, ecs::Entity b)
	{
		const Transform& aTransform = ecs::GetComponent<Transform>(a);
		const Transform& bTransform = ecs::GetComponent<Transform>(b);

		return sqrt(pow(bTransform.position.x - aTransform.position.x, 2) + pow(bTransform.position.y - aTransform.position.y, 2)
			+ pow(bTransform.position.z - aTransform.position.z, 2));
	}

	//Get the angle of b with a as the origin
	//TODO Fix this to work in 3D
	float TransformSystem::Angle(ecs::Entity a, ecs::Entity b)
	{
		const Transform& aTransform = ecs::GetComponent<Transform>(a);
		const Transform& bTransform = ecs::GetComponent<Transform>(b);

		return Degrees(atan2f(bTransform.position.y - aTransform.position.y,
		                          bTransform.position.x - aTransform.position.x));
	}

	//Calculate the global transform of an entity, this is not a reference and does not affect the original transform
	Transform TransformSystem::GetGlobalTransform(ecs::Entity entity)
	{
		const Transform& transform = ecs::GetComponent<Transform>(entity);

		Transform globalTransform = transform;
		ecs::Entity currentParent = transform.parent;
		//Iterate through each parent adding their transform
		while (currentParent != 0)
		{
			const Transform& parentTransform = ecs::GetComponent<Transform>(currentParent);

			globalTransform.position += parentTransform.position;
			globalTransform.rotation += parentTransform.rotation;
			globalTransform.scale += parentTransform.scale;

			currentParent = parentTransform.parent;
		}

		return globalTransform;
	}

	glm::mat4 TransformSystem::GetLocalTranformMatrix(ecs::Entity entity)
	{
		const Transform& transform = ecs::GetComponent<Transform>(entity);

		//Create the transform matrix
		glm::mat4 transformMatrix = glm::mat4(1.0f);
		//Position
		transformMatrix = glm::translate(transformMatrix, transform.position.ToGlm());
		//Apply euler rotations in desired order
		ApplyRotation(transformMatrix, transform.rotation, transform.rotationOrder);
		//Scale
		transformMatrix = glm::scale(transformMatrix, transform.scale.ToGlm());
		//Pivot
		transformMatrix = glm::translate(transformMatrix, -transform.pivot.ToGlm());

		return transformMatrix;
	}

	//Get the global Transform matrix of an entity after all parent transforms have been applied
	glm::mat4 TransformSystem::GetGlobalTransformMatrix(ecs::Entity entity)
	{
		const Transform& transform = ecs::GetComponent<Transform>(entity);

		//Go through all the parents and add them to a list
		std::vector<ecs::Entity> parents;
		parents.push_back(entity);
		ecs::Entity currentParent = transform.parent;
		while (currentParent != 0)
		{
			parents.push_back(currentParent);

			currentParent = ecs::GetComponent<Transform>(currentParent).parent;
		}

		//Create the transform matrix
		glm::mat4 transformMatrix = GetLocalTranformMatrix(parents.back());

		//Go through each parent from root up
		for (int i = parents.size() - 2; i >= 0; i--)
		{
			transformMatrix *= GetLocalTranformMatrix(parents[i]);
		}

		return transformMatrix;
	}

	//Applies transforms to vertices and returns the transformed vertices, takes rotation in degrees
	std::vector<Vector3> TransformSystem::ApplyTransforms(const std::vector<Vector3>& vertices, const Transform& transform)
	{
		//Create the transform matrix
		glm::mat4 transformMatrix = glm::mat4(1.0f);
		//Position
		transformMatrix = glm::translate(transformMatrix, transform.position.ToGlm());
		//Apply euler rotations in desired order
		ApplyRotation(transformMatrix, transform.rotation, transform.rotationOrder);
		//Scale
		transformMatrix = glm::scale(transformMatrix, transform.scale.ToGlm());
		//Pivot
		transformMatrix = glm::translate(transformMatrix, -transform.pivot.ToGlm());

		std::vector<Vector3> transformedVerts;
		//For each vertice apply the transforms
		for (int i = 0; i < vertices.size(); i++)
		{
			//Many type conversions later we have applied transform matrix
			glm::vec3 glmVert = glm::vec3(glm::vec4(vertices[i].ToGlm(), 0) * transformMatrix);
			Vector3 vert(glmVert.x, glmVert.y, glmVert.z);
			transformedVerts.push_back(vert);
		}

		return transformedVerts;
	}

	//Applies transforms to 2D vertices and returns the transformed vertices, takes z rotation in degrees
	std::vector<Vector2> TransformSystem::ApplyTransforms2D(const std::vector<Vector2>& vertices, const Transform& transform, double rotation)
	{
		std::vector<Vector2> transformedVerts;
		//For each vertice apply scale and rotation
		for (int i = 0; i < vertices.size(); i++)
		{
			//Apply transform to the polygon collider
			Vector2 transformedVert = vertices[i];
			//Pivot
			transformedVert -= transform.pivot;
			//Rotate
			const float angle = Radians(rotation);
			transformedVert.x = vertices[i].x * cosf(angle) - vertices[i].y * sinf(angle);
			transformedVert.y = vertices[i].x * sinf(angle) + vertices[i].y * cosf(angle);
			//Scale
			transformedVert *= transform.scale;
			//Move
			transformedVert += transform.position;

			transformedVerts.push_back(transformedVert);
		}
		return transformedVerts;
	}

	void TransformSystem::ApplyRotation(glm::mat4& mat, Vector3 eulers, RotationOrder order)
	{
		switch (order)
		{
			case XYZ:
				mat = glm::rotate(mat, (float)glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case XZY:
				mat = glm::rotate(mat, (float)glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case YXZ:
				mat = glm::rotate(mat, (float)glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			case ZXY:
				mat = glm::rotate(mat, (float)glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case YZX:
				mat = glm::rotate(mat, (float)glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case ZYX:
				mat = glm::rotate(mat, (float)glm::radians(eulers.z), glm::vec3(0.0f, 0.0f, 1.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.y), glm::vec3(0.0f, 1.0f, 0.0f));
				mat = glm::rotate(mat, (float)glm::radians(eulers.x), glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			default:
				break;
		}
	}
}
