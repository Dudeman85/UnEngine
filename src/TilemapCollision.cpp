#include "TilemapCollision.h"

namespace une
{
	/*
	//Find any collision layer tiles in a box
	std::vector<Vector2> Tilemap::CheckCollisionBox(Vector2 topLeft, Vector2 bottomRight)
	{
		//Log all tiles
		std::vector<Vector2> collisions;
		if (collisionLayer.empty())
		{
			return collisions;
		}

		//Calculate the x and y index bounds
		int xMin = floor((topLeft.x + position.x) / tileSize.x);
		int xMax = ceil((bottomRight.x + position.x) / tileSize.x);
		int yMin = floor((-topLeft.y + position.y) / tileSize.y);
		int yMax = ceil((-bottomRight.y + position.y) / tileSize.y);

		//Check every position inside the box
		for (int x = xMin; x < xMax; x++)
		{
			for (int y = yMin; y < yMax; y++)
			{
				//Check bounds
				if (x < collisionLayer.size() && x >= 0 && y < collisionLayer[0].size() && y >= 0)
				{
					//If tile is in collision layer log it
					if (collisionLayer[x][y] != 0)
					{
						collisions.push_back(Vector2(x, y));
					}
				}
			}
		}
		return collisions;
	}
	*/

	/*
	//Returns the collision layers tile ID at x and y
	unsigned int Tilemap::checkCollision(float x, float y)
	{
		if (collisionLayer.empty())
			return 0;

		int xIndex = floor((x + position.x) / tileSize.x);
		int yIndex = floor((-y + position.y) / tileSize.y);

		//Check out of bounds
		if (xIndex >= collisionLayer.size() || yIndex >= collisionLayer[0].size() || xIndex < 0 || yIndex < 0)
			return 0;

		return collisionLayer[xIndex][yIndex];
	}
	*/
}