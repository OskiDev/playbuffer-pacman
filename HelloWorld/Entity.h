#pragma once
#include "Play.h"
#include "Grid.h"
#include <algorithm>

enum EntityType
{
	ENT_NONE,
	ENT_PACMAN,
	ENT_BLINKY,
	ENT_INKY,
	ENT_PINKY,
	ENT_CLYDE
};

enum MoveDirection
{
	MOVE_UP,
	MOVE_LEFT,
	MOVE_DOWN,
	MOVE_RIGHT,
	MOVE_NONE
};

struct EntityInfo
{
	int entityId{ -1 };
	Play::GameObject* entityObj{ nullptr };
	MoveDirection moveDirection{ MOVE_UP };
	MoveDirection queuedDirection{ MOVE_NONE };
	int movementTimer{ MOVEMENT_TIME };
	int targetGridX{ 0 };
	int targetGridY{ 0 };
	bool canMove{ true };
	std::function<void()> pathfindingFunc{ nullptr };
	bool teleportingFromLeft{ false };
	bool teleportingFromRight{ false };
};

class Entity
{
public: 
	//Functions
	void setup(EntityType givenType, std::string spriteName, int spawnX, int spawnY, MoveDirection startingDirection, int givenMovementTimer);
	int getNextGridSpaceIndex(MoveDirection givenDirection);
	void updateRotation();
	virtual bool canMoveToNextGridSpace(bool directionIsParameter = false, MoveDirection givenMoveDirection = MOVE_NONE);
	bool isTeleporting();
	void teleportHandle();
	bool canTeleportLeft();
	bool canTeleportRight();
	virtual void selectNewLocation();
	void smoothMovement();
	Play::Point2f getDirectionVector(MoveDirection givenDirection, int scalar = 1);
	Play::Point2f decrementDirectionVector(MoveDirection direction, Play::Point2f givenVector);
	MoveDirection getReverseDirection(MoveDirection currentDirection);

	//Variables
	int id{ -1 };
	Play::GameObject* obj{ nullptr };
	EntityType type{ ENT_NONE };
	MoveDirection moveDirection{ MOVE_NONE };
	MoveDirection queuedDirection{ MOVE_NONE };
	int defaultMovementTimer{ 0 };
	int movementTimer{ 0 };
	int queuedMovementTimer{ -1 };
	int targetGridX{ 0 };
	int targetGridY{ 0 };
	bool canMove{ 0 };
	bool teleportingFromLeft{ false };
	bool teleportingFromRight{ false };
};