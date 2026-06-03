#pragma once
#include "Entity.h"
#include "Pacman.h"

class Ghost : public Entity
{
public:
	//Functions
	Ghost();
	void setup(EntityType givenType, int spawnX, int spawnY, MoveDirection startingDirection, int givenMovementTimer, Pacman* givenPacmanPtr, Ghost* givenBlinkyPtr);
	void selectNewLocation() override;
	void findNewDirection(Play::Point2f targetVector);
	int blinkyFindTargetIndex();
	int inkyFindTargetIndex();
	int pinkyFindTargetIndex();
	int clydeFindTargetIndex();
	int getFleeIndex();
	int getExitHouseIndex();
	int getScatterIndex();
	int checkSpecialCases(bool changeVars = false);
	bool canMoveToNextGridSpace(bool directionIsParameter = false, MoveDirection givenMoveDirection = MOVE_NONE) override;

	//Variables
	std::function<int()> PathfindingFunc;
	Pacman* PacmanPtr;
	Ghost* BlinkyPtr;
	bool FleeingToHouse;
	bool CanLeaveHouse;
	bool IsInHouse;
	std::unordered_map<EntityType, std::string> GhostSpriteMap{ {ENT_BLINKY, "ghostblinky"}, {ENT_INKY, "ghostinky"}, {ENT_PINKY, "ghostpinky"}, {ENT_CLYDE, "ghostclyde"} };
};