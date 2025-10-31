#pragma once
#include "ECS.h"
#include "Tilemap.h"
#include "Transform.h"

namespace une
{
	//Tilemap Collider Component
	ECS_REGISTER_COMPONENT(TilemapCollider)
	struct TilemapCollider
	{
		Tilemap* tilemap;
	};

	ECS_REGISTER_SYSTEM(TilemapCollisionSystem, Transform, TilemapCollider)
	class TilemapCollisionSystem : public ecs::System
	{
	public:
		//Get the tilemap coords of a tile with a collider at this point in world coords
		static Vector2Int GetCollisionTile(ecs::Entity entity, Vector2 pos);
		//Get the tilemap coords of all tiles with colliders within a rectangle in world coords
		static std::vector<Vector2Int> GetCollisionTiles(ecs::Entity entity, Vector2 topLeft, Vector2 bottomRight);
	};
}
