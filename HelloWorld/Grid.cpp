#include "Grid.h"
#include "GameInstance.h"

void Grid::setup()
{
	for (int i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++)
	{
		if (i <= (sizeof(defaultGridSegments)))
		{
			segmentArray[i] = SegmentType(defaultGridSegments[i]);
		}
		else
		{
			segmentArray[i] = SEG_EMPTY;
		}
	}

	countNumberOfDots();
}

void Grid::draw()
{
	for (int i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++)
	{
		if (segmentSpriteMap.contains(segmentArray[i]))
		{
			Play::DrawSprite(segmentSpriteMap[segmentArray[i]].data(), indexToPixelLoc(i), 0);
		}
	}
}

void Grid::countNumberOfDots()
{
	gameInstance.gameState.initialDotCounter = 0;

	for (SegmentType& segment : segmentArray)
	{
		if (segment == SEG_DOT || segment == SEG_POWERDOT)
		{
			gameInstance.gameState.initialDotCounter++;
		}
	}
}

int Grid::coordsToIndex(int x, int y)
{
	return ((y * GRID_WIDTH) + x);
}

void Grid::indexToCoords(int index, int& x, int& y)
{
	x = index % GRID_WIDTH;
	y = index / GRID_WIDTH;
}

Play::Point2f Grid::indexToPixelLoc(int index)
{
	int segmentX, segmentY;
	indexToCoords(index, segmentX, segmentY);
	return Play::Point2f(segmentX * SEGMENT_SIZE, segmentY * SEGMENT_SIZE);
}

void Grid::spawnFruit()
{
	bool spawnLoop = true;
	while (spawnLoop)
	{
		int randIndex = gameInstance.randomIntInRange(0, (GRID_HEIGHT * GRID_WIDTH) - 1);
		if (segmentArray[randIndex] == SEG_DOT)
		{
			segmentArray[randIndex] = SEG_FRUIT;
			spawnLoop = false;
		}
	}
}