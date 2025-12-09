#pragma once

#include <vector>

#include "ECS.h"
#include "Transform.h"
#include "Collision.h"

namespace une
{
	enum Direction { up = 0, right = 1, down = 2, left = 3 };

	//Rigidbody component
	ECS_REGISTER_COMPONENT(Rigidbody)
	struct Rigidbody
	{
		Vector3 velocity;
		//Mass of the rigidbody in Danny's
		float mass = 1;
		//How much gravity affects the rigidbody
		float gravityScale = 1;
		//How much linear drag should be applied
		float drag = 0;
		//Aka bounciness how much velocity will be preserved after a collision, recommended between 0-1
		float restitution = 1;
		//If true, this will not be effected by outside forces calculations
		bool kinematic = false;
	};

	//Physics System, Requires Rigidbody and Transform components
	ECS_REGISTER_SYSTEM(PhysicsSystem, Transform, Rigidbody)
	class PhysicsSystem : public ecs::System
	{
	public:
		//Update the physics system, call this every frame
		void Update();

		//COLLISION RESOLUTION:

		//Temporary (permanent) function to solve a collision. Does not affect rotation, Returns 0 on success, >0 on trigger, and <0 on failure
		static int SimpleSolveCollision(const Collision& collision);

		//Solve a collision between an entity and a tilemap, Returns 0 on success, >0 on trigger, and <0 on failure
		static int SolveTilemapCollision(std::vector<Collision> collisions);

		//UTILITY:

		//Move an entity while checking for collision, assuming entity has collider
		static void Move(ecs::Entity entity, Vector3 amount, int steps = 1);

		//Add an impulse to entity, does not include deltaTime
		static inline void Impulse(ecs::Entity entity, Vector3 velocity);

		//Add force to entity
		static inline void AddForce(ecs::Entity entity, Vector3 velocity);

		//How many steps should be used for movements, bigger is more accurate but slower
		int step = 1;
		Vector3 gravity;
	};
}