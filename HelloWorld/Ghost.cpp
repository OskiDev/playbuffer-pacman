#include "Ghost.h"
#include "GameInstance.h"

Ghost::Ghost()
{
	PathfindingFunc = nullptr;
	PacmanPtr = nullptr;
	BlinkyPtr = nullptr;
	FleeingToHouse = false;
	CanLeaveHouse = false;
	IsInHouse = true;
}

void Ghost::setup(EntityType givenType, int spawnX, int spawnY, MoveDirection startingDirection, int givenMovementTimer, Pacman* givenPacmanPtr, Ghost* givenBlinkyPtr)
{
	switch (givenType)
	{
	case ENT_BLINKY:
		PathfindingFunc = std::bind_front(&Ghost::blinkyFindTargetIndex, this);
		break;

	case ENT_INKY:
		PathfindingFunc = std::bind_front(&Ghost::inkyFindTargetIndex, this);
		break;

	case ENT_PINKY:
		PathfindingFunc = std::bind_front(&Ghost::pinkyFindTargetIndex, this);
		break;

	case ENT_CLYDE:
		PathfindingFunc = std::bind_front(&Ghost::clydeFindTargetIndex, this);
		break;
		
	case ENT_NONE:
	case ENT_PACMAN:
		break;
	}
	PacmanPtr = givenPacmanPtr;
	BlinkyPtr = givenBlinkyPtr;
	Entity::setup(givenType, GhostSpriteMap[givenType], spawnX, spawnY, startingDirection, givenMovementTimer);
}

void Ghost::selectNewLocation()
{
	checkSpecialCases(true);
	this->findNewDirection(gameInstance.grid.indexToPixelLoc(PathfindingFunc()));
	Entity::selectNewLocation();
}

void Ghost::findNewDirection(Play::Point2f targetVector)
{
	if (!isTeleporting())
	{
		MoveDirection moveDirArray[4] = { MOVE_UP, MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN};
		MoveDirection closestDirection = MOVE_NONE;
		float closestDirectionDistance = INFINITY;

		for (MoveDirection direction : moveDirArray)
		{
			int nextIndex = getNextGridSpaceIndex(direction);
			//if nextSegment is a valid move and that the ghost is allowed to go that way
			if ((canMoveToNextGridSpace(true, direction) || canTeleportLeft() || canTeleportRight()) && getReverseDirection(moveDirection) != direction)
			{
				//if distance is smaller or distance is equal but direction is same (stops ghost from arbitrarily changing directions for no gain)
				if ((gameInstance.grid.indexToPixelLoc(nextIndex).getWrappedDistanceTo(targetVector)) < closestDirectionDistance ||
					((gameInstance.grid.indexToPixelLoc(nextIndex).getWrappedDistanceTo(targetVector) == closestDirectionDistance && direction == moveDirection)))
				{
					closestDirectionDistance = gameInstance.grid.indexToPixelLoc(nextIndex).getWrappedDistanceTo(targetVector);
					closestDirection = direction;
				}
				if (gameInstance.grid.segmentArray[getNextGridSpaceIndex(direction)] == SEG_GHOSTDOOR)
				{
					closestDirection = direction;
				}
			}
		}
		//Play::DrawSpriteCircle(targetVector, 1, "powerdot", Play::cGreen);

		moveDirection = closestDirection;
	}
}

int Ghost::blinkyFindTargetIndex()
{
	if (checkSpecialCases() != -1)
	{
		return checkSpecialCases();
	}

	return gameInstance.grid.coordsToIndex(PacmanPtr->targetGridX, PacmanPtr->targetGridY);
}

int Ghost::inkyFindTargetIndex()
{
	if (checkSpecialCases() != -1)
	{
		return checkSpecialCases();
	}


	Play::Point2f aheadLocation = getDirectionVector(PacmanPtr->moveDirection, 2);
	int px = PacmanPtr->targetGridX + int(aheadLocation.x);
	int py = PacmanPtr->targetGridY + int(aheadLocation.y);
	Play::Point2f pacAheadPixel = { px * SEGMENT_SIZE + SEGMENT_HALFSIZE, py * SEGMENT_SIZE + SEGMENT_HALFSIZE };

	float vx = pacAheadPixel.x - BlinkyPtr->obj->pos.x;
	float vy = pacAheadPixel.y - BlinkyPtr->obj->pos.y;

	float targetX = BlinkyPtr->obj->pos.x + vx * 2.0f;
	float targetY = BlinkyPtr->obj->pos.y + vy * 2.0f;

	int tileX = int(targetX / SEGMENT_SIZE);
	int tileY = int(targetY / SEGMENT_SIZE);

	tileX = std::clamp(tileX, 0, GRID_WIDTH - 1);
	tileY = std::clamp(tileY, 0, GRID_HEIGHT - 1);

	return gameInstance.grid.coordsToIndex(tileX, tileY);
}

int Ghost::pinkyFindTargetIndex()
{
	if (checkSpecialCases() != -1)
	{
		return checkSpecialCases();
	}


	Play::Point2f pacmanDirVector = getDirectionVector(PacmanPtr->moveDirection, 2);
	int x = PacmanPtr->targetGridX + int(pacmanDirVector.x);
	int y = PacmanPtr->targetGridY + int(pacmanDirVector.y);
	x = std::clamp(x, 0, GRID_WIDTH - 1);
	y = std::clamp(y, 0, GRID_HEIGHT - 1);
	return gameInstance.grid.coordsToIndex(x, y);
}

int Ghost::clydeFindTargetIndex()
{
	if (checkSpecialCases() != -1)
	{
		return checkSpecialCases();
	}

	float eightTileSize = SEGMENT_SIZE * 8;
	float distanceBetween = obj->pos.getDistanceTo(PacmanPtr->obj->pos);
	
	if (distanceBetween >= eightTileSize)
	{
		return gameInstance.grid.coordsToIndex(PacmanPtr->targetGridX, PacmanPtr->targetGridY);
	}

	//bottom left corner
	return gameInstance.grid.coordsToIndex(1, 1);
}

int Ghost::getFleeIndex()
{
	switch (type)
	{
	case ENT_BLINKY:
		return gameInstance.grid.coordsToIndex(12, 16);

	case ENT_INKY:
		return gameInstance.grid.coordsToIndex(13, 16);

	case ENT_PINKY:
		return gameInstance.grid.coordsToIndex(14, 16);

	case ENT_CLYDE:
		return gameInstance.grid.coordsToIndex(15, 16);
	}
	
	return 0;
}

int Ghost::getExitHouseIndex()
{
	return gameInstance.grid.coordsToIndex(13, 19);
}

int Ghost::getScatterIndex()
{
	switch (type)
	{
	case ENT_BLINKY:
		return gameInstance.grid.coordsToIndex(26, 29);

	case ENT_INKY:
		return gameInstance.grid.coordsToIndex(26, 1);

	case ENT_PINKY:
		return gameInstance.grid.coordsToIndex(1, 29);

	case ENT_CLYDE:
		return gameInstance.grid.coordsToIndex(1, 1);
	}
}

int Ghost::checkSpecialCases(bool changeVars)
{
	if (changeVars)
	{
		int compX;
		int compY;
		if (FleeingToHouse)
		{
			gameInstance.grid.indexToCoords(getFleeIndex(), compX, compY);
			if (compX == targetGridX && compY == targetGridY)
			{
				FleeingToHouse = false;
				defaultMovementTimer = MOVEMENT_TIME;
				IsInHouse = true;
			}
		}
		else if (CanLeaveHouse)
		{
			gameInstance.grid.indexToCoords(getExitHouseIndex(), compX, compY);
			if (compX == targetGridX && compY == targetGridY)
			{
				CanLeaveHouse = false;
				IsInHouse = false;
			}
		}
	}
	else
	{
		if (FleeingToHouse)
		{
			return getFleeIndex();
		}
		else if (CanLeaveHouse)
		{
			return getExitHouseIndex();
		}
		else if (gameInstance.gameState.ghostScatter)
		{
			return getScatterIndex();
		}

	}
	return -1;
}

bool Ghost::canMoveToNextGridSpace(bool directionIsParameter, MoveDirection givenMoveDirection)
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
		if ((CanLeaveHouse || FleeingToHouse) && gameInstance.grid.segmentArray[getNextGridSpaceIndex(localMoveDirection)] == SEG_GHOSTDOOR)
		{
			return true;
		}
		return (gameInstance.grid.segmentArray[getNextGridSpaceIndex(localMoveDirection)] != SEG_WALL && gameInstance.grid.segmentArray[getNextGridSpaceIndex(localMoveDirection)] != SEG_GHOSTDOOR);
	}

	return false;
}
