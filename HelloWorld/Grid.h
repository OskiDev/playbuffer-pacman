#pragma once
#include "Play.h"
#include "GameState.h"

enum SegmentType
{
	SEG_EMPTY,
	SEG_WALL,
	SEG_DOT,
	SEG_POWERDOT,
	SEG_FRUIT,
	SEG_GHOSTDOOR
};

class Grid
{
public:
	//Functions
	void setup();
	void draw();
	void countNumberOfDots();
	int coordsToIndex(int x, int y);
	void indexToCoords(int index, int& x, int& y);
	Play::Point2f indexToPixelLoc(int index);
	void spawnFruit();

	//Variables
	SegmentType segmentArray[GRID_WIDTH * GRID_HEIGHT]{};
	std::unordered_map<SegmentType, std::string> segmentSpriteMap{ {SEG_WALL, "wall"}, {SEG_DOT, "dot"}, {SEG_POWERDOT, "powerdot"}, {SEG_FRUIT, "fruit"}, {SEG_GHOSTDOOR, "door"} };
};