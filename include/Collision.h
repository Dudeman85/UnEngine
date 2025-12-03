#pragma once

#include <vector>
#include <array>
#include <functional>

#include "ECS.h"
#include "Vector.h"
#include "Transform.h"

namespace une
{
	//Collision Event struct, not a component
	struct Collision
	{
		enum class Type { miss, collision, trigger, tilemapCollision, tilemapTrigger };

		Type type;

		//The entity which instigated the collision
		ecs::Entity a;
		//The entity which was subject to the collision
		ecs::Entity b;
		//In a tilemap collision this will be the tile GID
		uint16_t tileGID = 0;

		//The point of collision, will always be a vertice of one collider
		Vector3 point;
		//The normal of the collision surface, will always be the normal of a side of the other collider than point
		Vector3 normal;
		//Minimum Translation Vector is the smallest translation needed to end overlap
		Vector3 mtv;
	};

	//Polygon Collider component
	ECS_REGISTER_COMPONENT(PolygonCollider)
	struct PolygonCollider
	{
		//The vertices of the polygon making up the collider, going clockwise. The vertices must form a convex polygon
		std::vector<Vector2> vertices;
		//Callback function on collision
		std::function<void(Collision)> callback;
		//Should the collider only act as a trigger
		bool trigger = false;
		//The layer of the collider, behavior is determined by the collision layer matrix
		int layer = 0;
		//Override the rotation of the collider, (0-360)degrees. This is useful if attaching a 2D collider to a 3D model
		float rotationOverride = -1;
		//Draw this collider
		bool visualise = false;
		//The axis-aligned bounding box of the collider. This is updated automatically and is set in world coordinates
		std::array<float, 4> bounds;
	};

	//Collision System, Requires Transform and PolygonCollider
	ECS_REGISTER_SYSTEM(CollisionSystem, Transform, PolygonCollider)
	class CollisionSystem : public ecs::System
	{
	public:
		enum class LayerInteraction { all, none, collisions, triggers };

		//Called every frame
		void Update();

		//Checks collision between entity a and every other entity and tilemap, Returns the collisions from the perspective of a, and calls every applicable callback function
		std::vector<Collision> CheckCollision(ecs::Entity a);
		//Checks for collision between a tilemap collision layer and an entity. Does not call callbacks
		std::vector<Collision> CheckTilemapCollision(ecs::Entity entity);
		//Check Entity-Entity collision. Does not call callbacks
		Collision CheckEntityCollision(ecs::Entity a, ecs::Entity b);

		//Check SAT intersection between two convex polygons, Expects Vertices to have Transforms applied
		static Collision SATIntersect(std::vector<Vector2> aVerts, std::vector<Vector2> bVerts);
		//Checks if a and b bounds are intersecting
		static bool AABBIntersect(ecs::Entity a, ecs::Entity b);
		//Update the AABB of the polygon collider
		static void UpdateAABB(ecs::Entity entity);

		//Set the collision layer of a tile id
		inline void SetTileCollisionLayer(unsigned int tileID, int layer);
		//Get the collision layer of a tile id, defaults to 0
		inline int GetTileCollisionLayer(unsigned int tileID);
		//Sets the interaction state between two layers
		inline void SetLayerInteraction(int layer1, int layer2, LayerInteraction interaction);
		//Get the interaction type between two collision layers
		inline LayerInteraction GetLayerInteraction(int layer1, int layer2);

	private:
		std::unordered_map<int, std::unordered_map<int, LayerInteraction>> layerCollisionMatrix;
		std::unordered_map<unsigned int, int> tileIDTolayer;
		std::unordered_map<unsigned int, bool> tileIDToTrigger;
	};
}