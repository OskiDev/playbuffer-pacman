#include "Entity.h"
#include "GameInstance.h"

void Entity::setup(EntityType givenType, std::string spriteName, int spawnX, int spawnY, MoveDirection startingDirection, int givenMovementTimer)
{
	type = givenType;
	targetGridX = spawnX;
	targetGridY = spawnY; 
	moveDirection = startingDirection;
	defaultMovementTimer = givenMovementTimer;
	movementTimer = defaultMovementTimer;
	id = Play::CreateGameObject(type, { 0, 0 }, 7, spriteName.data());
	obj = &Play::GetGameObject(id);
	selectNewLocation();
}

int Entity::getNextGridSpaceIndex(MoveDirection givenDirection)
{
	Play::Point2f dir = getDirectionVector(givenDirection);
	return gameInstance.grid.coordsToIndex(targetGridX + int(dir.x), targetGridY + int(dir.y));
}

void Entity::updateRotation()
{
	switch (moveDirection)
	{
	case MOVE_NONE:
		obj->rotation = 0.0f;
		break;

	case MOVE_UP:
		obj->rotation = 4.71f;
		break;

	case MOVE_LEFT:
		obj->rotation = 0.0f;
		break;

	case MOVE_RIGHT:
		obj->rotation = 3.14f;
		break;

	case MOVE_DOWN:
		obj->rotation = 1.54f;
		break;
	}
}

bool Entity::canMoveToNextGridSpace(bool directionIsParameter, MoveDirection givenMoveDirection)
{
	MoveDirection localMoveDirection;

	if (directionIsParameter)
	{
		localMoveDirection = givenMoveDirection;
	}
	else
	{
		localMoveDirection = moveDirection;
	}

	if (localMoveDirection != MOVE_NONE)
	{
		return (gameInstance.grid.segmentArray[getNextGridSpaceIndex(localMoveDirection)] != SEG_WALL && gameInstance.grid.segmentArray[getNextGridSpaceIndex(localMoveDirection)] != SEG_GHOSTDOOR);
	}

	return false;
}

bool Entity::isTeleporting()
{
	return (teleportingFromLeft || teleportingFromRight);
}

void Entity::teleportHandle()
{
	Play::Point2f teleportPosition;

	if (canTeleportLeft())
	{
		teleportingFromLeft = true;
		canMove = true;
		return;
	}

	if (teleportingFromLeft)
	{
		teleportPosition = gameInstance.grid.indexToPixelLoc(gameInstance.grid.coordsToIndex(27, 16));
		obj->pos = { teleportPosition.x + SEGMENT_SIZE + SEGMENT_HALFSIZE, teleportPosition.y + SEGMENT_HALFSIZE };
		targetGridX = 27;
		targetGridY = 16;
		teleportingFromLeft = false;
		canMove = true;
		return;
	}

	if (canTeleportRight())
	{
		teleportingFromRight = true;
		canMove = true;
		return;
	}

	if (teleportingFromRight)
	{
		teleportPosition = gameInstance.grid.indexToPixelLoc(gameInstance.grid.coordsToIndex(0, 16));
		obj->pos = { teleportPosition.x - SEGMENT_SIZE + SEGMENT_HALFSIZE, teleportPosition.y + SEGMENT_HALFSIZE };
		targetGridX = 0;
		targetGridY = 16;
		teleportingFromRight = false;
		canMove = true;
		return;
	}
}

bool Entity::canTeleportLeft()
{
	return (targetGridX == 0 && targetGridY == 16 && !isTeleporting() && moveDirection == MOVE_LEFT);
}

bool Entity::canTeleportRight()
{
	return (targetGridX == 27 && targetGridY == 16 && !isTeleporting() && moveDirection == MOVE_RIGHT);
}

void Entity::selectNewLocation()
{
	if (queuedMovementTimer != -1)
	{
		defaultMovementTimer = queuedMovementTimer;
		queuedMovementTimer = -1;
	}

	if (!isTeleporting())
	{
		Play::Point2f currentPos = gameInstance.grid.indexToPixelLoc(gameInstance.grid.coordsToIndex(targetGridX, targetGridY));
		obj->pos = { currentPos.x + 10, currentPos.y + SEGMENT_HALFSIZE };
	}

	canMove = false;

	if (canMoveToNextGridSpace() && !isTeleporting())
	{
		if (type == ENT_PACMAN)
		{
			updateRotation();
		}
		switch (moveDirection)
		{
		case MOVE_UP:
			targetGridY++;
			targetGridY = std::clamp(targetGridY, 0, GRID_HEIGHT - 1);
			canMove = true;
			break;

		case MOVE_LEFT:
			targetGridX--;
			targetGridX = std::clamp(targetGridX, 0, GRID_WIDTH - 1);
			canMove = true;
			break;

		case MOVE_RIGHT:
			targetGridX++;
			targetGridX = std::clamp(targetGridX, 0, GRID_WIDTH - 1);
			canMove = true;
			break;

		case MOVE_DOWN:
			targetGridY--;
			targetGridY = std::clamp(targetGridY, 0, GRID_HEIGHT - 1);
			canMove = true;
			break;
		}
	}
	else
	{
		teleportHandle();
	}
}

void Entity::smoothMovement()
{
	switch (moveDirection)
	{
	case MOVE_UP:
		obj->pos = { obj->pos.x, obj->pos.y + (float(SEGMENT_SIZE) / float(defaultMovementTimer)) };
		break;

	case MOVE_LEFT:
		obj->pos = { obj->pos.x - (float(SEGMENT_SIZE) / float(defaultMovementTimer)), obj->pos.y };
		break;

	case MOVE_RIGHT:
		obj->pos = { obj->pos.x + (float(SEGMENT_SIZE) / float(defaultMovementTimer)), obj->pos.y };
		break;

	case MOVE_DOWN:
		obj->pos = { obj->pos.x, obj->pos.y - (float(SEGMENT_SIZE) / float(defaultMovementTimer)) };
		break;
	}
}

Play::Point2f Entity::getDirectionVector(MoveDirection givenDirection, int scalar)
{
	switch (givenDirection)
	{
	case MOVE_UP:
		return { 0, scalar };

	case MOVE_LEFT:
		return { -scalar, 0 };

	case MOVE_RIGHT:
		return { scalar, 0 };

	case MOVE_DOWN:
		return { 0, -scalar };
	}

	return { 0, 0 };
}

Play::Point2f Entity::decrementDirectionVector(MoveDirection direction, Play::Point2f givenVector)
{
	switch (direction)
	{
	case MOVE_UP:
		return { givenVector.x, givenVector.y - 1 };

	case MOVE_LEFT:
		return { givenVector.x + 1, givenVector.y };

	case MOVE_RIGHT:
		return { givenVector.x - 1, givenVector.y };

	case MOVE_DOWN:
		return { givenVector.x, givenVector.y + 1 };
	};

	return { 0, 0 };
}

MoveDirection Entity::getReverseDirection(MoveDirection currentDirection)
{
	return (MoveDirection)((currentDirection + 2) % 4);
}